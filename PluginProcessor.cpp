/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
GuideLinesCompAudioProcessor::GuideLinesCompAudioProcessor() : AudioProcessor(
    BusesProperties()
    .withInput("Input", juce::AudioChannelSet::stereo(), true)
    .withOutput("Output", juce::AudioChannelSet::stereo(), true)
),
params(apvts)
{
    lowCutFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
}

GuideLinesCompAudioProcessor::~GuideLinesCompAudioProcessor()
{
}

//==============================================================================
const juce::String GuideLinesCompAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GuideLinesCompAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool GuideLinesCompAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool GuideLinesCompAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double GuideLinesCompAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GuideLinesCompAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int GuideLinesCompAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GuideLinesCompAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String GuideLinesCompAudioProcessor::getProgramName(int index)
{
    return {};
}

void GuideLinesCompAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void GuideLinesCompAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    params.prepareToPlay(sampleRate);
    params.reset();

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = juce::uint32(samplesPerBlock);
    spec.numChannels = 2;

    lowCutFilter.prepare(spec);
    lowCutFilter.reset();
    compA.prepare(spec);
    compA.reset();

    compB.prepare(spec);
    compB.reset();

    lastLowCut = -1.f;
}

void GuideLinesCompAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GuideLinesCompAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto mono = juce::AudioChannelSet::mono();
    const auto stereo = juce::AudioChannelSet::stereo();
    const auto in = layouts.getMainInputChannelSet();
    const auto out = layouts.getMainOutputChannelSet();

    DBG("isBusesLayoutSupported, in: " << in.getDescription() << ", out: " << out.getDescription());

    if ((in == mono && out == mono) ||
        (in == mono && out == stereo) ||
        (in == stereo && out == stereo))
        return true;

    return false;
}
#endif

void GuideLinesCompAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    initializeProcessing(buffer);
    updateBypassState();

    params.update();
    params.smoothen();
    updateLowCutFilter();
    updateMappedCompressorParameters();

    rmsTotalGainReductionLeft.reset();
    rmsTotalGainReductionRight.reset();

    // Route input/output
    juce::AudioBuffer<float> mainInput = getBusBuffer(buffer, true, 0);
    juce::AudioBuffer<float> mainOutput = getBusBuffer(buffer, false, 0);

    const int numInputChannels = mainInput.getNumChannels();
    const int numOutputChannels = mainOutput.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int ch = numInputChannels; ch < numOutputChannels; ++ch)
        mainOutput.clear(ch, 0, numSamples);

    for (int ch = 0; ch < juce::jmin(numInputChannels, numOutputChannels); ++ch)
        mainOutput.copyFrom(ch, 0, mainInput, ch, 0, numSamples);

    juce::dsp::AudioBlock<float> block(mainOutput);
    juce::dsp::ProcessContextReplacing<float> ctx(block);

    mainOutput.applyGain(compressInputGainSmoother.getNextValue());

    // --- Measure & compute input RMS + peak BEFORE processing
    updateRMSLevels(mainOutput, rmsInputLevelLeft, rmsInputLevelRight);
    updatePeakLevels(mainOutput, peakInputLevelLeft, peakInputLevelRight);
    rmsInputLevelLeft.computeRMS();
    rmsInputLevelRight.computeRMS();

    lowCutFilter.process(ctx);
    compA.processCompression(ctx);

    // --- Measure & compute interstage RMS
    updateRMSLevels(mainOutput, rmsInterstageLevelLeft, rmsInterstageLevelRight);
    rmsInterstageLevelLeft.computeRMS();
    rmsInterstageLevelRight.computeRMS();

    compB.processCompression(ctx);

    outputGainProcessor.setGainLinear(params.outputGain);
    outputGainProcessor.process(ctx);

    // --- Measure & compute output RMS + peak AFTER all processing
    updateRMSLevels(mainOutput, rmsOutputLevelLeft, rmsOutputLevelRight);
    updatePeakLevels(mainOutput, peakOutputLevelLeft, peakOutputLevelRight);
    rmsOutputLevelLeft.computeRMS();
    rmsOutputLevelRight.computeRMS();

    // --- Compute gain reduction using the stored values
    const float rmsInputL = rmsInputLevelLeft.getValue();
    const float rmsInputR = rmsInputLevelRight.getValue();
    const float rmsInterL = rmsInterstageLevelLeft.getValue();
    const float rmsInterR = rmsInterstageLevelRight.getValue();
    const float rmsOutputL = rmsOutputLevelLeft.getValue();
    const float rmsOutputR = rmsOutputLevelRight.getValue();
    
    float totalGR_L = juce::Decibels::gainToDecibels(rmsInputL) - juce::Decibels::gainToDecibels(rmsOutputL);
    float totalGR_R = juce::Decibels::gainToDecibels(rmsInputR) - juce::Decibels::gainToDecibels(rmsOutputR);

    rmsTotalGainReductionLeft.updateDirect(totalGR_L);
    rmsTotalGainReductionRight.updateDirect(totalGR_R);

    compAGainReductionDbLeft.store(
        juce::Decibels::gainToDecibels(rmsInterL) -
        juce::Decibels::gainToDecibels(rmsInputL));
    compAGainReductionDbRight.store(
        juce::Decibels::gainToDecibels(rmsInterR) -
        juce::Decibels::gainToDecibels(rmsInputR));

    compBGainReductionDbLeft.store(
        juce::Decibels::gainToDecibels(rmsOutputL) -
        juce::Decibels::gainToDecibels(rmsInterL));
    compBGainReductionDbRight.store(
        juce::Decibels::gainToDecibels(rmsOutputR) -
        juce::Decibels::gainToDecibels(rmsInterR));

    rmsTotalGainReductionLeft.computeAverage();
    rmsTotalGainReductionRight.computeAverage();

    DBG("compAGRLeft: " << compAGainReductionDbLeft.load()
        << "\ncompAGRRight: " << compAGainReductionDbRight.load()
        << "\nCompBGRLeft: " << compBGainReductionDbLeft.load()
        << "\nCompBGRight: " << compBGainReductionDbRight.load()
        << "\nTotalGRLeft: " << rmsTotalGainReductionLeft.getValue()
        << "\nTotalGRRight: " << rmsTotalGainReductionRight.getValue());

#if JUCE_DEBUG
    protectYourEars(buffer);
#endif
}

//==============================================================================
bool GuideLinesCompAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GuideLinesCompAudioProcessor::createEditor()
{
    return new GuideLinesCompAudioProcessorEditor(*this);
}

//==============================================================================
void GuideLinesCompAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void GuideLinesCompAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuideLinesCompAudioProcessor();
}


juce::AudioProcessorParameter* GuideLinesCompAudioProcessor::getBypassParameter() const
{
    return params.bypassParam;
}

void GuideLinesCompAudioProcessor::initializeProcessing(juce::AudioBuffer<float>& buffer)
{
    juce::ScopedNoDenormals noDenormals;

    int totalNumInputChannels = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    params.update();
}

void GuideLinesCompAudioProcessor::updateBypassState()
{
    if (params.bypassed && bypassFade >= 1.0f)
        return;

    if (params.bypassed && bypassFade < 1.0f)
        bypassFade += bypassFadeInc;
    else if (!params.bypassed && bypassFade > 0.0f)
        bypassFade -= bypassFadeInc;
}
void GuideLinesCompAudioProcessor::updateLowCutFilter()
{
    if (params.lowCut != lastLowCut)
    {
        lowCutFilter.setCutoffFrequency(params.lowCut);
        lastLowCut = params.lowCut;
    }
}

void GuideLinesCompAudioProcessor::updateMappedCompressorParameters()
{
    //--- Raw parameter inputs ---
    float controlValue = juce::jlimit(1.0f, 100.0f, params.control);
    float compressValue = juce::jlimit(1.0f, 100.0f, params.compression);

    //--- Normalized values ---
    float normCompress = compressValue / 100.0f;
    float normControl = controlValue / 100.0f;

    //--- Input gain (from compression value) ---
    float inputGainDb = juce::jmap(normCompress, -3.0f, 12.0f);
    float inputGainLin = juce::Decibels::decibelsToGain(inputGainDb);
    compressInputGainSmoother.setTargetValue(inputGainLin);

    //--- Compressor envelope shaping (from control value) ---
    float mappedAttack = juce::mapToLog10(normControl, 60.0f, 1.0f);
    float mappedRelease = juce::jmap(controlValue, 0.0f, 100.0f, 55.0f, 100.0f);
    float mappedThreshold = juce::jmap(controlValue, 0.0f, 100.0f, -12.0f, -24.0f);

    //--- Ratio scaling (from compression value) ---
    float mappedRatio = juce::jmap(compressValue, 0.0f, 100.0f, 2.0f, 10.0f);

    //--- Apply smoothed values ---
    controlAttackASmoother.setTargetValue(mappedAttack);
    controlReleaseASmoother.setTargetValue(mappedRelease);
    controlThresholdASmoother.setTargetValue(mappedThreshold);
    compressRatioASmoother.setTargetValue(mappedRatio);

    //--- Update visible state ---
    controlAttackA = mappedAttack;
    controlReleaseA = mappedRelease;
    compressThresholdA = mappedThreshold;
    compressRatioA = mappedRatio;

    //--- Input to compressor ---
    compA.updateCompressorSettings(
        controlAttackASmoother.getNextValue(),
        controlReleaseASmoother.getNextValue(),
        compressRatioASmoother.getNextValue(),
        controlThresholdASmoother.getNextValue());

    DBG("Attack: " << mappedAttack
        << "\nRelease: " << mappedRelease
        << "\nThreshold: " << mappedThreshold
        << "\nRatio: " << mappedRatio
        << "\nInput gain: " << inputGainDb);

}

void GuideLinesCompAudioProcessor::updateRMSLevels(const juce::AudioBuffer<float>& buffer,
    RmsMeasurement& rmsLevelLeft,
    RmsMeasurement& rmsLevelRight)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int ch = 0; ch < numChannels; ++ch)
    {
        const float* data = buffer.getReadPointer(ch);
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = data[i];
            if (ch == 0)
                rmsLevelLeft.update(sample);
            else if (ch == 1)
                rmsLevelRight.update(sample);
        }
    }
}

void GuideLinesCompAudioProcessor::updatePeakLevels(
    const juce::AudioBuffer<float>& buffer,
    Measurement& peakLevelLeft,
    Measurement& peakLevelRight)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int ch = 0; ch < numChannels; ++ch)
    {
        const float* data = buffer.getReadPointer(ch);

        for (int i = 0; i < numSamples; ++i)
        {
            const float absSample = std::fabs(data[i]);

            if (ch == 0)
                peakLevelLeft.updateIfGreater(absSample);
            else if (ch == 1)
                peakLevelRight.updateIfGreater(absSample);
        }
    }
}

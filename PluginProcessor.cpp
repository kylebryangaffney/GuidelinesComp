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

void GuideLinesCompAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GuideLinesCompAudioProcessor::getProgramName (int index)
{
    return {};
}

void GuideLinesCompAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GuideLinesCompAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    params.prepareToPlay(sampleRate);
    params.reset();


    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = juce::uint32(samplesPerBlock);
    spec.numChannels = 2;

    lowCutFilter.prepare(spec);
    lowCutFilter.reset();

    lastLowCut = -1.f;
}

void GuideLinesCompAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GuideLinesCompAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void GuideLinesCompAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    initializeProcessing(buffer);
    updateBypassState();

    params.update();
    params.smoothen();
    updateLowCutFilter();

    juce::AudioBuffer<float> mainInput = getBusBuffer(buffer, true, 0); // Input bus 0
    juce::AudioBuffer<float> mainOutput = getBusBuffer(buffer, false, 0); // Output bus 0

    const int numInputChannels = mainInput.getNumChannels();
    const int numOutputChannels = mainOutput.getNumChannels();
    const int numSamples = buffer.getNumSamples();


    // Clear unused output channels
    for (int ch = numInputChannels; ch < numOutputChannels; ++ch)
        mainOutput.clear(ch, 0, numSamples);

    // Copy input to output for processing
    for (int ch = 0; ch < juce::jmin(numInputChannels, numOutputChannels); ++ch)
        mainOutput.copyFrom(ch, 0, mainInput, ch, 0, numSamples);

    juce::dsp::AudioBlock<float>  block(mainOutput);
    juce::dsp::ProcessContextReplacing<float> ctx(block);

    lowCutFilter.process(ctx);

    outputGainProcessor.setGainLinear(params.outputGain);
    outputGainProcessor.process(ctx);

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
    return new GuideLinesCompAudioProcessorEditor (*this);
}

//==============================================================================
void GuideLinesCompAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GuideLinesCompAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
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

void GuideLinesCompAudioProcessor::updateBypassFade()
{
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
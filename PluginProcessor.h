/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>

#include "Service/Parameters.h"
#include "Service/ProtectYourEars.h"
#include "DSP/CompressorUnit.h"
#include "DSP/OptoCompressorUnit.h"
#include "Service/Measurement.h"
#include "Service/RmsMeasurement.h"


//==============================================================================
/**
*/
class GuideLinesCompAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    GuideLinesCompAudioProcessor();
    ~GuideLinesCompAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    juce::AudioProcessorParameter* getBypassParameter() const override;

    juce::AudioProcessorValueTreeState apvts{
       *this, nullptr, "Parameters", Parameters::createParameterLayout()
    };

    Parameters params;
    CompressorUnit compA;
    OptoCompressorUnit compB;

    RmsMeasurement rmsInputLevelLeft;
    RmsMeasurement rmsInputLevelRight;
    Measurement peakInputLevelRight;
    Measurement peakInputLevelLeft;
    RmsMeasurement rmsOutputLevelLeft;
    RmsMeasurement rmsOutputLevelRight;
    Measurement peakOutputLevelRight;
    Measurement peakOutputLevelLeft;

private:

    juce::dsp::StateVariableTPTFilter<float> lowCutFilter;
    float lastLowCut = -1.f;

    juce::dsp::Gain<float> outputGainProcessor;

    float bypassFade = 1.0f;
    float bypassFadeInc = 0.0f;
    bool  isBypassing = false;

    float controlAttackA = 50.0f;
    float compressThresholdA = -12.f;
    float controlReleaseA = 55.0f;
    float compressRatioA = 2.0f;

    std::atomic<float> compAGainReductionDbLeft{ 0.0f };
    std::atomic<float> compAGainReductionDbRight{ 0.0f };

    RmsMeasurement rmsInterstageLevelLeft;
    RmsMeasurement rmsInterstageLevelRight;

    std::atomic<float> compBGainReductionDbLeft{ 0.0f };
    std::atomic<float> compBGainReductionDbRight{ 0.0f };

    juce::LinearSmoothedValue<float> compressInputGainSmoother = 1.0f;
    juce::LinearSmoothedValue<float> controlAttackASmoother = 50.0f;
    juce::LinearSmoothedValue<float> controlThresholdASmoother = -12.f;
    juce::LinearSmoothedValue<float> compressRatioASmoother = 2.0f;
    juce::LinearSmoothedValue<float> controlReleaseASmoother = 55.0f;

    void initializeProcessing(juce::AudioBuffer<float>& buffer);

    void updateBypassState();
    void updateBypassFade();
    void updateLowCutFilter();
    void updateMappedCompressorParameters();

    void updateRMSLevels(const juce::AudioBuffer<float>& buffer, RmsMeasurement& rmsLevelLeft, RmsMeasurement& rmsLevelRight);
    void updatePeakLevels(const juce::AudioBuffer<float>& buffer, Measurement& peakLevelLeft, Measurement& peakLevelRight);
    void resetMeterLevels();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuideLinesCompAudioProcessor)
};



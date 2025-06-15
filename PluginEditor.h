/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/LookAndFeel.h"
#include "GUI/RotaryKnob.h"
#include "GUI/AsymmetricalRotaryKnob.h"
#include "GUI/LevelMeter.h"
#include "GUI/GainReductionMeter.h"

//==============================================================================
/**
*/
class GuideLinesCompAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    GuideLinesCompAudioProcessorEditor(GuideLinesCompAudioProcessor&);
    ~GuideLinesCompAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:

    MainLookAndFeel mainLF;

    GuideLinesCompAudioProcessor& audioProcessor;
    RotaryKnob lowCutKnob{ "Low Cut", audioProcessor.apvts, lowCutParamID };
    RotaryKnob controlKnob{ "Control", audioProcessor.apvts, controlParamID };
    RotaryKnob compressionKnob{ "Compress", audioProcessor.apvts, compressionParamID };
    AsymmetricalRotaryKnob outputGainKnob{ "Output", audioProcessor.apvts, outputGainParamID };

    LevelMeter inputMeter{
        audioProcessor.peakInputLevelLeft, audioProcessor.peakInputLevelRight,
        audioProcessor.rmsInputLevelLeft, audioProcessor.rmsInputLevelRight };

    LevelMeter outputMeter{
        audioProcessor.peakOutputLevelLeft, audioProcessor.peakOutputLevelRight,
        audioProcessor.rmsOutputLevelLeft, audioProcessor.rmsOutputLevelRight };

    GainReductionMeter gRMeter{
        audioProcessor.rmsTotalGainReductionLeft, audioProcessor.rmsTotalGainReductionRight };

    juce::GroupComponent controlGroup;
    juce::GroupComponent meterGroup;
    juce::GroupComponent outputGroup;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuideLinesCompAudioProcessorEditor)
};

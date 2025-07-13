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
class GuideLinesCompAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    GuideLinesCompAudioProcessorEditor(GuideLinesCompAudioProcessor&);
    ~GuideLinesCompAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:

    MainLookAndFeel mainLF;

    GuideLinesCompAudioProcessor& audioProcessor;
    RotaryKnob lowCutKnob{ "Lo Cut", audioProcessor.apvts, lowCutParamID };
    RotaryKnob compressionKnob{ "Compress", audioProcessor.apvts, compressionParamID }; // Strength
    RotaryKnob controlKnob{ "Control", audioProcessor.apvts, controlParamID }; // Shape
    AsymmetricalRotaryKnob outputGainKnob{ "Out", audioProcessor.apvts, outputGainParamID };

    LevelMeter inputMeter{
        audioProcessor.peakInputLevelLeft, audioProcessor.peakInputLevelRight,
        audioProcessor.rmsInputLevelLeft, audioProcessor.rmsInputLevelRight };

    LevelMeter outputMeter{
        audioProcessor.peakOutputLevelLeft, audioProcessor.peakOutputLevelRight,
        audioProcessor.rmsOutputLevelLeft, audioProcessor.rmsOutputLevelRight };

    GainReductionMeter gRMeter{
        audioProcessor.rmsTotalGainReductionLeft, audioProcessor.rmsTotalGainReductionRight };

    juce::GroupComponent compressGroup;
    juce::GroupComponent controlGroup;
    juce::GroupComponent outputGroup;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuideLinesCompAudioProcessorEditor)
};

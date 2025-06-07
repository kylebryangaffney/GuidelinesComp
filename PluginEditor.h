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

    std::unique_ptr<RotaryKnob> lowCutKnob;
    std::unique_ptr<RotaryKnob> controlKnob;
    std::unique_ptr<RotaryKnob> compressionKnob;
    std::unique_ptr<AsymmetricalRotaryKnob> outputGainKnob;

    juce::GroupComponent controlGroup;
    juce::GroupComponent meterGroup;
    juce::GroupComponent outputGroup;

    MainLookAndFeel mainLF;

    GuideLinesCompAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuideLinesCompAudioProcessorEditor)
};

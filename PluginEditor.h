/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/LookAndFeel.h"
#include "Gui/RotaryKnob.h"

//==============================================================================
/**
*/
class GuideLinesCompAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    GuideLinesCompAudioProcessorEditor (GuideLinesCompAudioProcessor&);
    ~GuideLinesCompAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    //RotaryKnob lowCutKnob{ "Low Cut",         audioProcessor.apvts, lowCutParamID };
    //RotaryKnob controlKnob{ "Control",  audioProcessor.apvts, controlParamID };
    //RotaryKnob compressionKnob{ "Compression",    audioProcessor.apvts, compressionParamID };
    //RotaryKnob outputGainKnob{ "Output",      audioProcessor.apvts, outputGainParamID };

    std::unique_ptr<RotaryKnob> lowCutKnob;
    std::unique_ptr<RotaryKnob> controlKnob;
    std::unique_ptr<RotaryKnob> compressionKnob;

    juce::GroupComponent controlGroup;
    //juce::GroupComponent meterGroup;
    //juce::GroupComponent outputGroup;

    MainLookAndFeel mainLF;

    GuideLinesCompAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuideLinesCompAudioProcessorEditor)
};

/*
  ==============================================================================

    Parameters.h
    Created: 26 May 2025 9:34:26am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

const juce::ParameterID outputGainParamID{ "outputGain",       1 };
const juce::ParameterID lowCutParamID{ "lowCut",     1 };
const juce::ParameterID bypassParamID{ "bypass",     1 };

class Parameters
{
public:
    Parameters(juce::AudioProcessorValueTreeState& apvts);
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void update() noexcept;
    void prepareToPlay(double sampleRate) noexcept;
    void reset() noexcept;
    void smoothen() noexcept;

    //==============================================================================
    float outputGain = 0.f;
    float lowCut = 20.f;
    bool  bypassed = false;

    //==============================================================================
    juce::AudioParameterBool* bypassParam = nullptr;

private:
    // Parameter references
    juce::AudioParameterFloat* outputGainParam = nullptr;
    juce::AudioParameterFloat* lowCutParam = nullptr;


    // Smoothers
    juce::LinearSmoothedValue<float> outputGainSmoother;
    juce::LinearSmoothedValue<float> lowCutSmoother;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)
};

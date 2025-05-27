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
const juce::ParameterID controlParamID{ "control", 1 };
const juce::ParameterID compressionParamID{ "compression", 1 };
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
    float control = 1.0f;
    float compression = 1.0f;
    bool  bypassed = false;

    //==============================================================================
    juce::AudioParameterBool* bypassParam = nullptr;

private:
    // Parameter references
    juce::AudioParameterFloat* outputGainParam = nullptr;
    juce::AudioParameterFloat* lowCutParam = nullptr;
    juce::AudioParameterFloat* controlParam = nullptr;
    juce::AudioParameterFloat* compressionParam = nullptr;


    // Smoothers
    juce::LinearSmoothedValue<float> outputGainSmoother;
    juce::LinearSmoothedValue<float> lowCutSmoother;
    juce::LinearSmoothedValue<float> controlSmoother;
    juce::LinearSmoothedValue<float> compressionSmoother;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)
};

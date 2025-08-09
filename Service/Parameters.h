/*
  ==============================================================================

    Parameters.h
    Created: 26 May 2025 9:34:26am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/// Unique Parameter IDs used in the plugin's ValueTreeState
const juce::ParameterID outputGainParamID{ "outputGain", 1 };
const juce::ParameterID lowCutParamID{ "lowCut", 1 };
const juce::ParameterID controlParamID{ "control", 1 };
const juce::ParameterID compressionParamID{ "compression", 1 };
const juce::ParameterID bypassParamID{ "bypass", 1 };

//==============================================================================
/**
    Manages access, smoothing, and updating of plugin parameters.
    The Parameters class provides an abstraction over the AudioProcessorValueTreeState,
    allowing for smoother parameter handling, real-time updates, and UI/display values.
*/
class Parameters
{
public:
    /**
        Constructs the Parameters class by assigning pointers to the APVTS parameters.
        @param apvts A reference to the AudioProcessorValueTreeState where parameters are registered.
    */
    Parameters(juce::AudioProcessorValueTreeState& apvts);

    /**
        Creates and returns the layout of parameters used in the plugin.
        This is used during processor initialization to register all plugin parameters.
        @return The parameter layout to be passed to AudioProcessorValueTreeState.
    */
    [[nodiscard]]
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    /**
        Prepares all internal smoothers with the given sample rate.
        @param sampleRate The sample rate of the audio processing environment.
    */
    void prepareToPlay(double sampleRate) noexcept;

    /**
        Resets all parameter values and their smoothers to their default state.
    */
    void reset() noexcept;

    /**
        Updates target values for all smoothed parameters from the latest raw parameter values.
    */
    void update() noexcept;

    /**
        Advances the smoothed values by one processing frame and stores them for processing use.
    */
    void smoothen() noexcept;

    //==============================================================================
    /// The current smoothed output gain value (linear gain).
    float outputGain = 0.f;

    /// The current smoothed low-cut filter frequency in Hz.
    float lowCut = 20.f;

    /// The current smoothed value for the 'control' parameter (e.g., envelope shaping).
    float control = 1.f;

    /// The current smoothed value for the 'compression' parameter (e.g., input gain).
    float compression = 1.f;

    /// True if the effect is bypassed, false otherwise.
    bool bypassed = false;

    /// Direct access to the bypass parameter (for UI toggling or logic decisions).
    juce::AudioParameterBool* bypassParam = nullptr;

private:
    //==============================================================================
    /// Raw pointer to the output gain parameter in dB.
    juce::AudioParameterFloat* outputGainParam = nullptr;

    /// Raw pointer to the low-cut frequency parameter.
    juce::AudioParameterFloat* lowCutParam = nullptr;

    /// Raw pointer to the control parameter.
    juce::AudioParameterFloat* controlParam = nullptr;

    /// Raw pointer to the compression parameter.
    juce::AudioParameterFloat* compressionParam = nullptr;

    //==============================================================================
    /// Smoother for output gain to avoid sudden jumps in loudness.
    juce::LinearSmoothedValue<float> outputGainSmoother;

    /// Smoother for low-cut filter frequency.
    juce::LinearSmoothedValue<float> lowCutSmoother;

    /// Smoother for control parameter (e.g., attack/release shaping).
    juce::LinearSmoothedValue<float> controlSmoother;

    /// Smoother for compression parameter (e.g., input level control).
    juce::LinearSmoothedValue<float> compressionSmoother;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)
};

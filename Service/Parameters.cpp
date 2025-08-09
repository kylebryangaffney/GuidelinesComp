/*
  ==============================================================================

    Parameters.cpp
    Created: 26 May 2025 9:34:26am
    Author:  kyleb

  ==============================================================================
*/

#include "Parameters.h"

//==============================================================================
// Helper Functions
//==============================================================================

// Template to safely assign parameter pointers
template<typename T>
inline void castParameter(juce::AudioProcessorValueTreeState& apvts,
    const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(destination);
}

// UI: Float (Hz) -> Display String
static juce::String stringFromHz(float value, [[maybe_unused]] int)
{
    if (value < 1000.f)       return juce::String(int(value)) + " Hz";
    else if (value < 10000.f) return juce::String(value / 1000.f, 2) + " kHz";
    else                      return juce::String(value / 1000.f, 1) + " kHz";
}

// UI: String -> Float (Hz)
static float hzFromString(const juce::String& str)
{
    float value = str.getFloatValue();
    return value < 20.f ? value * 1000.f : value;
}

// UI: Float -> Display Decimal
static juce::String stringFromDecimal(float value, [[maybe_unused]] int)
{
    return juce::String(int(value));
}

// UI: String -> Float (Decimal)
static float decimalFromString(const juce::String& str)
{
    return str.getFloatValue();
}

// UI: Float (dB) -> Display String
static juce::String stringFromDecibels(float value, [[maybe_unused]] int)
{
    return juce::String(value, 1) + " dB";
}

//==============================================================================
// Parameters Implementation
//==============================================================================

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts)
{
    castParameter(apvts, outputGainParamID, outputGainParam);
    castParameter(apvts, lowCutParamID, lowCutParam);
    castParameter(apvts, bypassParamID, bypassParam);
    castParameter(apvts, controlParamID, controlParam);
    castParameter(apvts, compressionParamID, compressionParam);
}

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        controlParamID, "Control",
        juce::NormalisableRange<float>{ 0.0f, 100.0f },
        0.f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction(stringFromDecimal)
        .withValueFromStringFunction(decimalFromString)
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        compressionParamID, "Compression",
        juce::NormalisableRange<float>{ 0.0f, 100.0f },
        0.f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction(stringFromDecimal)
        .withValueFromStringFunction(decimalFromString)
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        lowCutParamID, "Low Cut",
        juce::NormalisableRange<float>{ 20.f, 1000.f, 1.f, 0.3f },
        20.f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction(stringFromHz)
        .withValueFromStringFunction(hzFromString)
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        outputGainParamID, "Output Gain",
        juce::NormalisableRange<float>{ -18.f, 12.f },
        0.f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction(stringFromDecibels)
    ));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        bypassParamID, "Bypass", false
    ));

    return layout;
}

void Parameters::prepareToPlay(double sampleRate) noexcept
{
    const double smoothingTime = 0.002;

    outputGainSmoother.reset(sampleRate, smoothingTime);
    lowCutSmoother.reset(sampleRate, smoothingTime);
    controlSmoother.reset(sampleRate, smoothingTime);
    compressionSmoother.reset(sampleRate, smoothingTime);
}

void Parameters::reset() noexcept
{
    outputGain = 0.f;
    lowCut = 20.f;
    control = 0.f;
    compression = 0.f;

    outputGainSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(outputGainParam->get()));
    lowCutSmoother.setCurrentAndTargetValue(lowCutParam->get());
    controlSmoother.setCurrentAndTargetValue(controlParam->get());
    compressionSmoother.setCurrentAndTargetValue(compressionParam->get());
}

void Parameters::update() noexcept
{
    outputGainSmoother.setTargetValue(juce::Decibels::decibelsToGain(outputGainParam->get()));
    lowCutSmoother.setTargetValue(lowCutParam->get());
    controlSmoother.setTargetValue(controlParam->get());
    compressionSmoother.setTargetValue(compressionParam->get());

    bypassed = bypassParam->get();
}

void Parameters::smoothen() noexcept
{
    outputGain = outputGainSmoother.getNextValue();
    lowCut = lowCutSmoother.getNextValue();
    control = controlSmoother.getNextValue();
    compression = compressionSmoother.getNextValue();
}

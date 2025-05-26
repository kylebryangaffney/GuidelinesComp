/*
  ==============================================================================

    Parameters.cpp
    Created: 26 May 2025 9:34:26am
    Author:  kyleb

  ==============================================================================
*/

#include "Parameters.h"

template<typename T>
static void castParameter(juce::AudioProcessorValueTreeState& apvts,
    const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(destination);
}


Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts)
{
    castParameter(apvts, outputGainParamID, outputGainParam);
    castParameter(apvts, lowCutParamID, lowCutParam);
    castParameter(apvts, bypassParamID, bypassParam);
}


juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        outputGainParamID, "Output Gain",
        juce::NormalisableRange<float>{ -18.f, 12.f },
        0.f,
        juce::AudioParameterFloatAttributes()
    ));


    layout.add(std::make_unique<juce::AudioParameterFloat>(
        lowCutParamID, "Low Cut",
        juce::NormalisableRange<float>{ 20.f, 1000.f, 1.f, 0.3f },
        20.f,
        juce::AudioParameterFloatAttributes()
    ));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        bypassParamID, "Bypass", false
    ));

    return layout;
}

void Parameters::prepareToPlay(double sampleRate) noexcept
{
    double duration = 0.0002;

    outputGainSmoother.reset(sampleRate, duration);
    lowCutSmoother.reset(sampleRate, duration);
}


void Parameters::reset() noexcept
{
    outputGain = 0.f;
    lowCut = 20.f;

    outputGainSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(outputGainParam->get()));
    lowCutSmoother.setCurrentAndTargetValue(lowCutParam->get());
}

void Parameters::update() noexcept
{
    outputGainSmoother.setTargetValue(juce::Decibels::decibelsToGain(outputGainParam->get()));
    lowCutSmoother.setTargetValue(lowCutParam->get());
    bypassed = bypassParam->get();
}

void Parameters::smoothen() noexcept
{
    outputGain = outputGainSmoother.getNextValue();
    lowCut = lowCutSmoother.getNextValue();
}

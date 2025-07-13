/*
  ==============================================================================

    AsymmetricalRotaryKnob.cpp
    Created: 7 Jun 2025 7:31:47am
    Author:  kyleb

  ==============================================================================
*/

#include "AsymmetricalRotaryKnob.h"


//==============================================================================
AsymmetricalRotaryKnob::AsymmetricalRotaryKnob(const juce::String& text,
    juce::AudioProcessorValueTreeState& apvts,
    const juce::ParameterID& parameterID)
    : RotaryKnob(text, apvts, parameterID, false)
{
    const float pi = juce::MathConstants<float>::pi;

    slider.setRotaryParameters(1.25f * pi, 2.5f * pi, true);
}
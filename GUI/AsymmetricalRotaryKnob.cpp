/*
  ==============================================================================

    AsymmetricalRotaryKnob.cpp
    Created: 7 Jun 2025 7:31:47am
    Author:  kyleb

  ==============================================================================
*/

#include "AsymmetricalRotaryKnob.h"

//==============================================================================
/**
 * @brief Constructs an AsymmetricalRotaryKnob and customizes its rotary parameters.
 *
 * Calls the base RotaryKnob constructor with drawFromMiddle set to false,
 * then sets the rotary parameters to start at 1.25pi and end at 2.5pi radians,
 * producing an asymmetrical arc range.
 *
 * @param text        The label text shown above the knob.
 * @param apvts       The AudioProcessorValueTreeState to attach the knob's slider to.
 * @param parameterID The parameter ID in the APVTS to bind to this knob.
 */
AsymmetricalRotaryKnob::AsymmetricalRotaryKnob(const juce::String& text,
    juce::AudioProcessorValueTreeState& apvts,
    const juce::ParameterID& parameterID)
    : RotaryKnob(text, apvts, parameterID, false)
{
    const float pi = juce::MathConstants<float>::pi;

    slider.setRotaryParameters(1.25f * pi, 2.5f * pi, true);
}

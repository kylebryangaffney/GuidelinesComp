/*
  ==============================================================================

    AsymmetricalRotaryKnob.h
    Created: 7 Jun 2025 7:31:47am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include "RotaryKnob.h"

/**
 * @class AsymmetricalRotaryKnob
 * @brief A rotary knob with an asymmetrical rotation arc.
 *
 * Inherits from RotaryKnob and adjusts the rotary parameters to produce
 * an asymmetrical control feel. Also overrides the number of ticks.
 */
class AsymmetricalRotaryKnob : public RotaryKnob
{
public:
    /**
     * @brief Constructs an AsymmetricalRotaryKnob and attaches it to a parameter.
     *
     * @param text        The label text shown above the knob.
     * @param apvts       The AudioProcessorValueTreeState to attach the knob's slider to.
     * @param parameterID The parameter ID in the APVTS to bind to this knob.
     */
    AsymmetricalRotaryKnob(const juce::String& text,
        juce::AudioProcessorValueTreeState& apvts,
        const juce::ParameterID& parameterID);

    /**
     * @copydoc RotaryKnob::getNumTicks
     * @note Overridden to return 10 for the asymmetrical tick layout.
     */
    virtual int getNumTicks() const noexcept { return 10; };
};

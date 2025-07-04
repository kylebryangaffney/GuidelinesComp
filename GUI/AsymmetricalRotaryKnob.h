/*
  ==============================================================================

    AsymmetricalRotaryKnob.h
    Created: 7 Jun 2025 7:31:47am
    Author:  kyleb

  ==============================================================================
*/

#pragma once


#include "RotaryKnob.h"

//==============================================================================
class AsymmetricalRotaryKnob : public RotaryKnob
{
public:
    AsymmetricalRotaryKnob(const juce::String& text,
        juce::AudioProcessorValueTreeState& apvts,
        const juce::ParameterID& parameterID);

    virtual int getNumTicks() const { return 18; }
    
};
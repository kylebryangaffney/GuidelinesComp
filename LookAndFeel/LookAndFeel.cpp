/*
  ==============================================================================
    LookAndFeel.cpp
    Created: 4 Jun 2025 8:10:41am
    Author:  kyleb
  ==============================================================================
*/
/**
 * @file LookAndFeel.cpp
 * @brief Definitions for the plugin's base Look-and-Feel classes.
 */

#include "LookAndFeel.h"

 //==============================================================================
 // MainLookAndFeel

MainLookAndFeel::MainLookAndFeel()
{
    // GroupComponent styling
    setColour(juce::GroupComponent::textColourId, Colors::Group::label);
    setColour(juce::GroupComponent::outlineColourId, Colors::Group::outline);
}

juce::Font MainLookAndFeel::getLabelFont([[maybe_unused]] juce::Label& label)
{
    // Centralized font selection (uses cached typeface in Fonts)
    return Fonts::getFont();
}

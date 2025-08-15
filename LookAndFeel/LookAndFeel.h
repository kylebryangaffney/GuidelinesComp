/*
  ==============================================================================
    LookAndFeel.h
    Created: 4 Jun 2025 8:10:41am
    Author:  kyleb
  ==============================================================================
*/
/**
 * @file LookAndFeel.h
 * @brief Declarations for the plugin's base Look-and-Feel classes and helpers.
 */

#pragma once

#include <JuceHeader.h>
#include "Colors.h"
#include "Fonts.h"

 //==============================================================================
 // Main Look and Feel

 /**
  * @class MainLookAndFeel
  * @brief base LookAndFeel that sets common colours and fonts.
  *
  * This L&F establishes shared styling (e.g., GroupComponent colours) and
  * provides a consistent label font via the @c Fonts helper.
  */
class MainLookAndFeel : public juce::LookAndFeel_V4
{
public:
    /**
     * @brief Constructs the base LookAndFeel and applies global colours.
     *
     * Sets colours for @c juce::GroupComponent labels and outlines using
     * values from @c Colors::Group.
     */
    MainLookAndFeel();

    /**
     * @brief Returns the default font for JUCE labels.
     * @param label Reference to the label requesting the font (unused).
     * @return A @c juce::Font created via @c Fonts::getFont().
     *
     * @note Override point for label font selection across the UI.
     */
    juce::Font getLabelFont([[maybe_unused]] juce::Label& label) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainLookAndFeel)
};

/*
  ==============================================================================

    Fonts.h
    Created: 15 Aug 2025 8:29:02am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>



//==============================================================================
// Font Style


/** @brief Font height used for meter tick labels. */
constexpr float tickFontHeight = 8.0f;


class Fonts
{
public:
    Fonts() = delete;
    static juce::Font getFont(float height = 14.f);

private:
    static const juce::Typeface::Ptr typeface;
};

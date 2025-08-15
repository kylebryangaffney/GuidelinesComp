/*
  ==============================================================================

    Fonts.cpp
    Created: 15 Aug 2025 8:29:02am
    Author:  kyleb

  ==============================================================================
*/

#include "Fonts.h"



//==============================================================================
// Fonts


/** @brief Process-wide cached LatoMedium typeface loaded from binary data. */
const juce::Typeface::Ptr Fonts::typeface = juce::Typeface::createSystemTypefaceFor(
    BinaryData::LatoMedium_ttf, BinaryData::LatoMedium_ttfSize);


/**
 * @brief Returns a styled font using the cached typeface.
 * @param height Desired font height in points/pixels (JUCE units).
 * @return A juce::Font configured with the cached typeface and height.
 */
juce::Font Fonts::getFont(float height)
{
    return juce::FontOptions(typeface)
        .withMetricsKind(juce::TypefaceMetricsKind::legacy)
        .withHeight(height);
}

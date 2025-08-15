/*
  ==============================================================================

    LAFCommon.h
    Created: 15 Aug 2025 8:33:57am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Colors.h"



//==============================================================================
// Helpers

/**
 * @brief Draws a vertical tick and centers a numeric label at the given x position.
 * @param g           Graphics context to draw into.
 * @param x           X coordinate where the tick is drawn.
 * @param tickTop     Y coordinate of the top of the tick line.
 * @param tickHeight  Height of the tick line in pixels.
 * @param label       Text to draw under the tick.
 * @param labelTop    Y coordinate of the top of the label area.
 * @param labelWidth  Width of the label bounding box (used for centering).
 * @param labelHeight Height of the label bounding box.
 */
namespace
{
    void drawTickWithLabel(juce::Graphics& g, int x, int tickTop, int tickHeight,
        const juce::String& label, int labelTop, int labelWidth, int labelHeight)
    {
        g.setColour(Colors::LevelMeter::tickLine);
        g.fillRect(x, tickTop, 1, tickHeight);

        g.setColour(Colors::LevelMeter::tickLabel);
        g.drawText(label,
            x - labelWidth / 2, labelTop, labelWidth, labelHeight,
            juce::Justification::centred);
    }
}


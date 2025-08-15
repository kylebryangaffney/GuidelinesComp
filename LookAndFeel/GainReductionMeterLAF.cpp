/*
  ==============================================================================

    GainReductionMeterLAF.cpp
    Created: 15 Aug 2025 8:30:34am
    Author:  kyleb

  ==============================================================================
*/

#include "GainReductionMeterLAF.h"


//==============================================================================
// GainReductionMeterLookAndFeel

/**
 * @brief Paints the full Gain Reduction meter: frame, bar and tick labels.
 *
 * Renders the meter background/border, a single horizontal GR bar, and evenly
 * spaced dB tick marks with numeric labels. Geometry is snapped to integers for
 * crisp 1-px lines. Uses the meter's mapping function to convert dB values to
 * pixel X positions.
 *
 * @param g      Graphics context to draw into.
 * @param meter  Source of layout and dB->pixel mapping (positionForLevel).
 */
void GainReductionMeterLookAndFeel::drawGainReductionMeter(juce::Graphics& g,
    const GainReductionMeter& meter) noexcept
{
    const auto  font = Fonts::getFont(tickFontHeight);
    constexpr float borderThickness = 1.0f;
    constexpr float borderRadius = 4.0f;

    const auto borderRect = meter.getLocalBounds().toFloat().reduced(7.0f);
    const auto innerRect = borderRect.reduced(10.0f);

    g.fillAll(Colors::LevelMeter::background);
    g.setColour(Colors::LevelMeter::border);
    g.drawRoundedRectangle(borderRect, borderRadius, borderThickness);
    g.setFont(font);

    // Bar settings
    constexpr int rmsHeight = 4;

    // Snap geometry to integers for crisp 1-px lines
    const int xLeft = juce::roundToInt(innerRect.getX());
    const int xRight = juce::roundToInt(innerRect.getRight());
    const int yTop = juce::roundToInt(innerRect.getY());
    const int w = xRight - xLeft;
    if (w <= 0) return;

    /// Mapper: dB -> pixel x
    const auto map = [xLeft, xRight, &meter](float db) noexcept -> int
        {
            return meter.positionForLevel(db, xLeft, xRight);
        };

    // Draw the GR bar (thin RMS-style bar that reflects current max GR level)
    drawRmsLevel(xLeft, yTop, w, rmsHeight, g, meter.getMaxRmsLevel(), map);

    // Ticks and labels
    const int  tickTop = yTop + 2;  // 2 px down from top of inner rect
    constexpr int tickHeight = 8;
    const int  labelTop = tickTop + tickHeight;
    constexpr int labelHeight = 12;
    constexpr int labelWidth = 27;

    for (float db = GainReductionMeter::maxdB;
        db >= GainReductionMeter::mindB - 1.0e-3f;
        db -= GainReductionMeter::stepdB)
    {
        const int x = map(db);
        drawTickWithLabel(g, x, tickTop, tickHeight,
            juce::String(int(db)),
            labelTop, labelWidth, labelHeight);
    }
}

/**
 * @brief Draws a horizontal gain-reduction bar segment between two mapped X positions.
 *
 * Computes the pixel positions for "no reduction" (typically far right) and the
 * current clamped GR level, clamps them to the bar bounds, and fills the span.
 *
 * @param x               Left bar bound in pixels.
 * @param y               Top bar bound in pixels.
 * @param width           Bar width in pixels.
 * @param height          Bar height in pixels.
 * @param g               Graphics context to draw into.
 * @param levelDB         Current gain-reduction level in dB (negative domain).
 * @param positionForLevel Callable mapping dB -> pixel X within [x, x + width].
 * @param fillColour      Colour used to fill the bar.
 */
void GainReductionMeterLookAndFeel::drawMeterBar(
    int x, int y, int width, int height, juce::Graphics& g, float levelDB,
    const std::function<int(float)>& positionForLevel, juce::Colour fillColour) noexcept
{
    if (width <= 0 || height <= 0)
        return;

    const float clampedDB = juce::jlimit(GainReductionMeter::mindB,
        GainReductionMeter::maxdB,
        levelDB);

    // X of “no reduction” (typically the far right) and X of current GR
    const int xNoReduction = juce::jlimit(x, x + width,
        positionForLevel(GainReductionMeter::maxdB));
    const int xCurrentGR = juce::jlimit(x, x + width,
        positionForLevel(clampedDB));

    const int barStart = std::min(xNoReduction, xCurrentGR);
    const int barEnd = std::max(xNoReduction, xCurrentGR);

    if (barEnd > barStart)
    {
        g.setColour(fillColour);
        g.fillRect(barStart, y, barEnd - barStart, height);
    }
}

/**
 * @brief Convenience wrapper to draw the GR bar using the meter's GR colour.
 *
 * @param x               Left bar bound in pixels.
 * @param y               Top bar bound in pixels.
 * @param width           Bar width in pixels.
 * @param height          Bar height in pixels.
 * @param g               Graphics context to draw into.
 * @param levelDB         Current gain-reduction level in dB.
 * @param positionForLevel Callable mapping dB -> pixel X within [x, x + width].
 */
void GainReductionMeterLookAndFeel::drawRmsLevel(
    int x, int y, int width, int height,
    juce::Graphics& g,
    float levelDB,
    const std::function<int(float)>& positionForLevel) noexcept
{
    drawMeterBar(x, y, width, height, g, levelDB, positionForLevel,
        Colors::LevelMeter::gainReduction);
}

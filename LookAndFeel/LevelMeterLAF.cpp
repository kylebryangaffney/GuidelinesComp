/*
  ==============================================================================

    LevelMeterLAF.cpp
    Created: 15 Aug 2025 8:30:19am
    Author:  kyleb

  ==============================================================================
*/

#include "LevelMeterLAF.h"



//==============================================================================
// LevelMeterLookAndFeel

/**
 * @brief Draws the full level meter, including peak and RMS bars, borders, background, and tick labels.
 * @param g       The JUCE graphics context to use for rendering.
 * @param meter   Reference to the LevelMeter component providing measurement data and scale mapping.
 *
 * @note This method draws both channels' peak and RMS meters, applies background/border styling,
 *       and places dB tick marks along the scale.
 */
void LevelMeterLookAndFeel::drawLevelMeter(juce::Graphics& g, const LevelMeter& meter) noexcept
{
    const auto font = Fonts::getFont(tickFontHeight);
    constexpr float borderThickness = 1.0f;
    constexpr float borderRadius = 4.0f;

    const auto borderRect = meter.getLocalBounds().toFloat().reduced(7.0f);
    const auto innerRect = borderRect.reduced(10.0f);

    // Draw background and border
    g.fillAll(Colors::LevelMeter::background);
    g.setColour(Colors::LevelMeter::border);
    g.drawRoundedRectangle(borderRect, borderRadius, borderThickness);
    g.setFont(font);

    // Bar layout configuration
    constexpr int barHeight = 7;
    constexpr int rmsHeight = 4;
    constexpr int barSpacing = 2; // space between bars

    // Calculate bar X geometry
    const int xLeft = juce::roundToInt(innerRect.getX());
    const int xRight = juce::roundToInt(innerRect.getRight());
    const int w = xRight - xLeft;

    // Skip drawing if width is invalid
    if (w <= 0)
        return;

    // Calculate bar Y positions
    const int yPeakL = juce::roundToInt(innerRect.getY());
    const int yPeakR = yPeakL + barHeight + barSpacing;
    const int yRmsL = yPeakL;
    const int yRmsR = yPeakR;

    // Lambda to map dB values to pixel X positions
    const auto map = [xLeft, xRight, &meter](float db) noexcept
        {
            return meter.positionForLevel(db, xLeft, xRight);
        };

    // Draw peak levels
    drawPeakLevel(xLeft, yPeakL, w, barHeight, g, meter.getPeakLevelL(), map);
    drawPeakLevel(xLeft, yPeakR, w, barHeight, g, meter.getPeakLevelR(), map);

    // Draw RMS levels
    drawRmsLevel(xLeft, yRmsL, w, rmsHeight, g, meter.getRmsLevelL(), map);
    drawRmsLevel(xLeft, yRmsR, w, rmsHeight, g, meter.getRmsLevelR(), map);

    // Draw tick marks and numeric labels
    const int tickTop = yPeakL + 2; // offset down from top
    constexpr int tickHeight = 14;
    const int labelTop = tickTop + tickHeight;
    constexpr int labelHeight = 12;
    constexpr int labelWidth = 25;

    for (float db = meter.maxdB; db >= meter.mindB; db -= meter.stepdB)
    {
        const int x = map(db);
        drawTickWithLabel(g, x, tickTop, tickHeight,
            juce::String(int(db)),
            labelTop, labelWidth, labelHeight);
    }
}

/**
 * @brief Draws a single horizontal meter bar segment for either RMS or peak readings.
 * @param x                  Left X coordinate of the bar.
 * @param y                  Top Y coordinate of the bar.
 * @param width              Width of the bar in pixels.
 * @param height             Height of the bar in pixels.
 * @param g                  JUCE graphics context for drawing.
 * @param levelDB            The signal level in decibels (dB).
 * @param positionForLevel   Function mapping dB values to X coordinates.
 * @param fillColour         Colour to fill for levels below 0 dBFS.
 *
 * @note Levels above 0 dBFS are drawn in Colors::LevelMeter::tooLoud.
 */
void LevelMeterLookAndFeel::drawMeterBar(int x, int y, int width, int height,
    juce::Graphics& g, float levelDB,
    const std::function<int(float)>& positionForLevel,
    juce::Colour fillColour) noexcept
{
    if (!std::isfinite(levelDB) || levelDB <= clampdB || width <= 0 || height <= 0)
        return;

    jassert(x + width >= x); // geometry sanity check

    // Clamp pixel position to the bar's region
    const auto clampToBar = [x, width, &positionForLevel](float db) noexcept -> int
        {
            return juce::jlimit(x, x + width, positionForLevel(db));
        };

    const int xStart = clampToBar(levelDB);
    const int xZero = clampToBar(0.0f);

    if (xStart > xZero)
    {
        // Below 0 dBFS region
        g.setColour(fillColour);
        g.fillRect(x, y, xZero - x, height);

        // Above 0 dBFS region
        g.setColour(Colors::LevelMeter::tooLoud);
        g.fillRect(xZero, y, xStart - xZero, height);
    }
    else
    {
        // Entire bar is below 0 dBFS
        g.setColour(fillColour);
        g.fillRect(x, y, xStart - x, height);
    }
}

/**
 * @brief Draws a peak level bar for one channel.
 * @param x                  Left X coordinate.
 * @param y                  Top Y coordinate.
 * @param width              Width of the peak meter bar.
 * @param height             Height of the peak meter bar.
 * @param g                  JUCE graphics context.
 * @param levelDB            Peak level in dB.
 * @param positionForLevel   Mapping function from dB to X pixel position.
 */
void LevelMeterLookAndFeel::drawPeakLevel(int x, int y, int width, int height,
    juce::Graphics& g,
    float levelDB,
    const std::function<int(float)>& positionForLevel) noexcept
{
    drawMeterBar(x, y, width, height, g, levelDB, positionForLevel,
        Colors::LevelMeter::peakLevelOK);
}

/**
 * @brief Draws an RMS level bar for one channel.
 * @param x                  Left X coordinate.
 * @param y                  Top Y coordinate.
 * @param width              Width of the RMS meter bar.
 * @param height             Height of the RMS meter bar.
 * @param g                  JUCE graphics context.
 * @param levelDB            RMS level in dB.
 * @param positionForLevel   Mapping function from dB to X pixel position.
 */
void LevelMeterLookAndFeel::drawRmsLevel(int x, int y, int width, int height,
    juce::Graphics& g,
    float levelDB,
    const std::function<int(float)>& positionForLevel) noexcept
{
    drawMeterBar(x, y, width, height, g, levelDB, positionForLevel,
        Colors::LevelMeter::rmsLevelOK);
}
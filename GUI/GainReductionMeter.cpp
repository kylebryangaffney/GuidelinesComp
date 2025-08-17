/*
  ==============================================================================

    GainReductionMeter.cpp
    Created: 15 Jun 2025 10:34:12am
    Author:  kyleb

  ==============================================================================
*/
#include "GainReductionMeter.h"
#include "../LookAndFeel/GainReductionMeterLAF.h"

/**
 * @brief Constructs a GainReductionMeter component.
 *
 * Stores references to the left and right RMS measurement objects,
 * attaches a custom LookAndFeel, sets the component to opaque, and
 * starts a timer for GUI updates.
 *
 * @param rmsL Reference to left channel RMS measurement.
 * @param rmsR Reference to right channel RMS measurement.
 */
GainReductionMeter::GainReductionMeter(const RmsMeasurement& rmsL, const RmsMeasurement& rmsR)
    : rmsMeasurementL(rmsL), rmsMeasurementR(rmsR)
{
    setLookAndFeel(GainReductionMeterLookAndFeel::get());
    setOpaque(true);
    startTimerHz(refreshRate);
    lastUpdateMs = juce::Time::getMillisecondCounterHiRes();
}

GainReductionMeter::~GainReductionMeter() = default;

/**
 * @brief Paints the meter using the assigned LookAndFeel.
 *
 * Delegates drawing to GainReductionMeterLookAndFeel.
 *
 * @param g The graphics context used for painting.
 */
void GainReductionMeter::paint(juce::Graphics& g)
{
    if (auto* lnf = dynamic_cast<GainReductionMeterLookAndFeel*>(&getLookAndFeel()))
        lnf->drawGainReductionMeter(g, *this);
    else
        jassertfalse;
}

/**
 * @brief Handles component resizing.
 *
 * Updates internal drawing bounds (minPos and maxPos) based on the
 * current width of the component.
 */
void GainReductionMeter::resized()
{
    const float width = float(getWidth());
    if (width < 10.f)
    {
        minPos = 4.f;
        maxPos = 10.f;
    }
    else
    {
        minPos = 4.f;
        maxPos = width - 4.f;
    }
}

/**
 * @brief Maps a dB value to a pixel coordinate.
 *
 * Clamps the dB level into [mindB, maxdB], then maps it linearly
 * into the pixel range [minX, maxX].
 *
 * @param dbLevel The input level in dB.
 * @param minX The minimum pixel coordinate.
 * @param maxX The maximum pixel coordinate.
 * @return Pixel coordinate corresponding to the input dB level.
 */
int GainReductionMeter::positionForLevel(float dbLevel, float minX, float maxX) const noexcept
{
    dbLevel = juce::jlimit(mindB, maxdB, dbLevel);
    return int(std::round(
        juce::jmap(dbLevel, mindB, maxdB, minX, maxX)
    ));
}

/**
 * @brief Returns the higher dB value between left and right channels.
 *
 * Used so the meter can display whichever channel is showing the
 * greatest gain reduction.
 *
 * @return The maximum dB value from left or right.
 */
float GainReductionMeter::getMaxRmsLevel() const
{
    if (dbRmsLevelL > dbRmsLevelR)
        return dbRmsLevelL;
    else
        return dbRmsLevelR;
}

/**
 * @brief Timer callback executed at the refresh rate.
 *
 * - Measures elapsed time since the last update (dt).
 * - Clamps dt into [0, 0.25] seconds to avoid extreme jumps.
 * - Reads linear RMS values from both channels.
 * - Converts them to dB with a floor at mindB.
 * - Calls updateLevel() for each channel using dt-aware smoothing.
 * - Triggers repaint() to update the GUI.
 */
void GainReductionMeter::timerCallback()
{
    const double nowMs = juce::Time::getMillisecondCounterHiRes();
    double dtSec = (nowMs - lastUpdateMs) / 1000.0;
    lastUpdateMs = nowMs;

    dtSec = juce::jlimit(0.0, 0.25, dtSec);

    const float linearL = rmsMeasurementL.getValue();
    const float linearR = rmsMeasurementR.getValue();

    const float dbL = juce::Decibels::gainToDecibels(linearL, mindB);
    const float dbR = juce::Decibels::gainToDecibels(linearR, mindB);

    updateLevel(dbL, rmsLevelL, dbRmsLevelL, (float)dtSec);
    updateLevel(dbR, rmsLevelR, dbRmsLevelR, (float)dtSec);

    repaint();
}

/**
 * @brief Updates a smoothed dB level with attack and release ballistics.
 *
 * - Clamps the incoming dB value within [mindB, maxdB].
 * - Computes attack and release alphas using the exponential form:
 *   alpha = 1 - exp(-dt / tau).
 * - Chooses attack alpha if the input is rising, release alpha if falling.
 * - Applies one-pole smoothing: smoothed += (new - smoothed) * alpha.
 * - Updates the output reference level.
 *
 * @param newDb The new input level in dB.
 * @param smoothedDb Reference to the smoothed dB value to update.
 * @param leveldB Reference to the published dB level.
 * @param dtSec Elapsed time since last update in seconds.
 */
void GainReductionMeter::updateLevel(float newDb, float& smoothedDb, float& leveldB, const float dtSec) const
{
    newDb = juce::jlimit(mindB, maxdB, newDb);

    const double aAtk = 1.0 - std::exp(-(double)dtSec / (double)attackT);
    const double aRel = 1.0 - std::exp(-(double)dtSec / (double)releaseT);

    const bool isRising = (newDb > smoothedDb);
    const double a = isRising ? aAtk : aRel;

    smoothedDb += (float)((newDb - smoothedDb) * a);
    leveldB = smoothedDb;
}

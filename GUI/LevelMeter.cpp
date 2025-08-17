/*
  ==============================================================================

    LevelMeter.cpp
    Created: 7 Jun 2025 7:39:22am
    Author:  kyleb

  ==============================================================================
*/

#include "LevelMeter.h"
#include "../LookAndFeel/LevelMeterLAF.h"

//==============================================================================
/**
    Constructs a LevelMeter with given measurement references.
*/
LevelMeter::LevelMeter(Measurement& measurementL_, Measurement& measurementR_,
    const RmsMeasurement& rmsMeasurementL_, const RmsMeasurement& rmsMeasurementR_)
    : measurementL(measurementL_), measurementR(measurementR_),
    rmsMeasurementL(rmsMeasurementL_), rmsMeasurementR(rmsMeasurementR_)
{
    setLookAndFeel(LevelMeterLookAndFeel::get());

    setOpaque(true);
    startTimerHz(refreshRate);
    lastUpdateMs = juce::Time::getMillisecondCounterHiRes();
}

/**
    Destructor.
*/
LevelMeter::~LevelMeter() = default;

//==============================================================================
/**
    Paints the LevelMeter using its LookAndFeel.

    If a `LevelMeterLookAndFeel` is assigned, its `drawLevelMeter` method is used.
    Otherwise, triggers a failed assertion.
*/
void LevelMeter::paint(juce::Graphics& g)
{
    if (auto* lnf = dynamic_cast<LevelMeterLookAndFeel*>(&getLookAndFeel()))
        lnf->drawLevelMeter(g, *this);
    else
        jassertfalse;
}

/**
    Handles resize events and recalculates meter bounds.
*/
void LevelMeter::resized()
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

//==============================================================================
/**
    Timer callback, called at `refreshRate`.

    - Computes elapsed time (`dtSec`) since the last update.
    - Updates smoothed peak levels for left/right channels.
    - Updates RMS levels from `RmsMeasurement`.
    - Repaints the component.
*/
void LevelMeter::timerCallback()
{
    const double nowMs = juce::Time::getMillisecondCounterHiRes();
    double dtSec = (nowMs - lastUpdateMs) / 1000.0;
    lastUpdateMs = nowMs;

    // Clamp dt to avoid jumps from long UI stalls
    dtSec = juce::jlimit(0.0, 0.25, dtSec);

    updateLevel(measurementL.readAndReset(), levelL, dbLevelL, (float)dtSec);
    updateLevel(measurementR.readAndReset(), levelR, dbLevelR, (float)dtSec);

    dbRmsLevelL = juce::Decibels::gainToDecibels(rmsMeasurementL.getValue(), mindB);
    dbRmsLevelR = juce::Decibels::gainToDecibels(rmsMeasurementR.getValue(), mindB);

    repaint();
}

/**
    Updates and smooths a single channel’s peak level.

    @param newLevel       New input peak level (linear).
    @param smoothedLevel  Smoothed peak level (linear) — modified in-place.
    @param leveldB        Smoothed peak level (dB) — modified in-place.
    @param dtSec          Elapsed time in seconds since last update.
*/
void LevelMeter::updateLevel(float newLevel, float& smoothedLevel,
    float& leveldB, float dtSec) const
{
    newLevel = juce::jlimit(clampLevel, 1.0f, newLevel);

    const double aAtk = 1.0 - std::exp(-(double)dtSec / (double)attackT);
    const double aRel = 1.0 - std::exp(-(double)dtSec / (double)releaseT);

    const bool   isRising = (newLevel > smoothedLevel);
    const double a = isRising ? aAtk : aRel;

    smoothedLevel += (float)((newLevel - smoothedLevel) * a);

    leveldB = juce::Decibels::gainToDecibels(smoothedLevel, mindB);
}

/**
    Maps a dB value into a horizontal pixel position.

    @param dbLevel  Input dB level.
    @param minX     Minimum horizontal position.
    @param maxX     Maximum horizontal position.
    @returns Pixel coordinate corresponding to `dbLevel`.
*/
int LevelMeter::positionForLevel(const float dbLevel,
    const float minX,
    const float maxX) const noexcept
{
    const float clamped = juce::jlimit(mindB, maxdB, dbLevel);
    return (int)std::lround(juce::jmap(clamped, mindB, maxdB, minX, maxX));
}

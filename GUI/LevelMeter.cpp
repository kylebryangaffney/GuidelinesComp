/*
  ==============================================================================

    LevelMeter.cpp
    Created: 7 Jun 2025 7:39:22am
    Author:  kyleb

  ==============================================================================
*/

#include "LevelMeter.h"
#include"../LookAndFeel/LevelMeterLAF.h"


//==============================================================================
LevelMeter::LevelMeter(Measurement& measurementL_, Measurement& measurementR_,
    RmsMeasurement& rmsMeasurementL_, RmsMeasurement& rmsMeasurementR_)
    : measurementL(measurementL_), measurementR(measurementR_),
    rmsMeasurementL(rmsMeasurementL_), rmsMeasurementR(rmsMeasurementR_)
{
    setLookAndFeel(LevelMeterLookAndFeel::get());

    dbLevelL = clampdB;
    dbLevelR = clampdB;
    dbRmsLevelL = clampdB;
    dbRmsLevelR = clampdB;

    setOpaque(true);
    startTimerHz(refreshRate);

    decay = 1.f - std::exp(-1.f / (float(refreshRate) * 1.f));
}


LevelMeter::~LevelMeter() = default;

//==============================================================================
void LevelMeter::paint(juce::Graphics& g)
{
    if (auto* lnf = dynamic_cast<LevelMeterLookAndFeel*>(&getLookAndFeel()))
        lnf->drawLevelMeter(g, *this);
    else
        jassertfalse;
}

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

void LevelMeter::timerCallback()
{
    // Peak: read peak for this interval, decay smoothly
    updateLevel(measurementL.readAndReset(), levelL, dbLevelL);
    updateLevel(measurementR.readAndReset(), levelR, dbLevelR);

    // RMS: get average for this interval
    dbRmsLevelL = juce::Decibels::gainToDecibels(std::max(rmsMeasurementL.getValue(), clampLevel));
    dbRmsLevelR = juce::Decibels::gainToDecibels(std::max(rmsMeasurementR.getValue(), clampLevel));

    repaint();
}

void LevelMeter::updateLevel(float newLevel, float& smoothedLevel, float& leveldB) const
{
    if (newLevel > smoothedLevel)
        smoothedLevel = newLevel;
    else
        smoothedLevel += (newLevel - smoothedLevel) * decay;

    if (smoothedLevel > clampLevel)
        leveldB = juce::Decibels::gainToDecibels(smoothedLevel);
    else
        leveldB = clampdB;
}


int LevelMeter::positionForLevel(float dbLevel, float minX, float maxX) const noexcept
{
    return int(std::round(
        juce::jmap(dbLevel, mindB, maxdB, minX, maxX)
    ));
}



/*
  ==============================================================================

    GainReductionMeter.cpp
    Created: 15 Jun 2025 10:34:12am
    Author:  kyleb

  ==============================================================================
*/
#include "GainReductionMeter.h"
#include "LookAndFeel.h"

GainReductionMeter::GainReductionMeter(RmsMeasurement& rmsL, RmsMeasurement& rmsR)
    : rmsMeasurementL(rmsL), rmsMeasurementR(rmsR)
{
    setLookAndFeel(GainReductionMeterLookAndFeel::get());
    setOpaque(true);
    startTimerHz(refreshRate);
    decay = 1.f - std::exp(-1.f / (float(refreshRate) * 0.4f));
}

GainReductionMeter::~GainReductionMeter() = default;

void GainReductionMeter::paint(juce::Graphics& g)
{
    if (auto* lnf = dynamic_cast<GainReductionMeterLookAndFeel*>(&getLookAndFeel()))
        lnf->drawGainReductionMeter(g, *this);
    else
        jassertfalse;
}

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

int GainReductionMeter::positionForLevel(float dbLevel) const noexcept
{
    return int(std::round(juce::jmap(dbLevel, mindB, maxdB, minPos, maxPos)));
}

float GainReductionMeter::getMaxRmsLevel() const
{
    if (dbRmsLevelL > dbRmsLevelR)
        return dbRmsLevelL;
    else
        return dbRmsLevelR;
}

void GainReductionMeter::timerCallback()
{
    float linearL = rmsMeasurementL.getValue();
    float linearR = rmsMeasurementR.getValue();

    float dbL = juce::Decibels::gainToDecibels(linearL, mindB);
    float dbR = juce::Decibels::gainToDecibels(linearR, mindB);

    updateLevel(dbL, rmsLevelL, dbRmsLevelL);
    updateLevel(dbR, rmsLevelR, dbRmsLevelR);

    repaint();
}

void GainReductionMeter::updateLevel(float newDb, float& smoothedDb, float& leveldB) const
{
    if (newDb > smoothedDb)
        smoothedDb = newDb;
    else
        smoothedDb += (newDb - smoothedDb) * decay;

    leveldB = smoothedDb;
}



/*
  ==============================================================================

    GainReductionMeter.cpp
    Created: 15 Jun 2025 10:34:12am
    Author:  kyleb

  ==============================================================================
*/

#include "GainReductionMeter.h"
#include"LookAndFeel.h"

//==============================================================================
GainReductionMeter::GainReductionMeter(RmsMeasurement& rmsMeasurementL_, RmsMeasurement& rmsMeasurementR_)
    : rmsMeasurementL(rmsMeasurementL_), rmsMeasurementR(rmsMeasurementR_)
{
    dbRmsLevelL = clampdB;
    dbRmsLevelR = clampdB;

    setOpaque(true);
    startTimerHz(refreshRate);

    decay = 1.f - std::exp(-1.f / (float(refreshRate) * 0.4f));

}


GainReductionMeter::~GainReductionMeter() = default;

//==============================================================================
void GainReductionMeter::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds();
    g.fillAll(Colors::LevelMeter::background);
    g.setFont(Fonts::getFont(10.f));

    drawRmsLevel(g, dbRmsLevelL, 0, 7);
    drawRmsLevel(g, dbRmsLevelR, 9, 7);

    // Draw ticks top (0 dB) to bottom (mindB)
    for (float db = maxdB; db <= mindB; db += stepdB)
    {
        int y = positionForLevel(db);
        g.setColour(Colors::LevelMeter::tickLine);
        g.fillRect(0, y, 16, 1);

        g.setColour(Colors::LevelMeter::tickLabel);
        g.drawSingleLineText(juce::String(int(db)), bounds.getWidth() - 2, y + 3, juce::Justification::right);
    }
}

void GainReductionMeter::resized()
{
    const float height = float(getHeight());

    if (height < 10.f)
    {
        maxPos = 4.f;
        minPos = 10.f; // safe fallback
    }
    else
    {
        maxPos = 4.f;
        minPos = height - 4.f;
    }
}

void GainReductionMeter::timerCallback()
{
    updateLevel(rmsMeasurementL.getValue(), rmsLevelL, dbRmsLevelL);
    updateLevel(rmsMeasurementR.getValue(), rmsLevelR, dbRmsLevelR);
    repaint();
}

void GainReductionMeter::drawRmsLevel(juce::Graphics& g, float levelDB, int x, int width)
{
    if (levelDB <= 0.f)
        return;

    float db = juce::jlimit(0.0f, mindB, levelDB); // levelDB should be positive

    int yTop = positionForLevel(maxdB);   // usually 0
    int yCurrent = positionForLevel(db);  // lower = more reduction (further down)

    g.setColour(Colors::LevelMeter::rmsLevelOK);
    g.fillRect(x, yTop, width, yCurrent - yTop);

}


void GainReductionMeter::updateLevel(float newLevel, float& smoothedLevel, float& leveldB) const
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


/*
  ==============================================================================

    LevelMeter.cpp
    Created: 7 Jun 2025 7:39:22am
    Author:  kyleb

  ==============================================================================
*/

#include "LevelMeter.h"
#include"LookAndFeel.h"


//==============================================================================
LevelMeter::LevelMeter(const Measurement& measurementL_, const Measurement& measurementR_,
    const RmsMeasurement& rmsMeasurementL_, const RmsMeasurement& rmsMeasurementR_)
    : measurementL(measurementL_), measurementR(measurementR_),
    rmsMeasurementL(rmsMeasurementL_), rmsMeasurementR(rmsMeasurementR_)
{
    dbLevelL = clampdB;
    dbLevelR = clampdB;
    dbRmsLevelL = clampdB;
    dbRmsLevelR = clampdB;

    setOpaque(true);
    startTimerHz(refreshRate);

    decay = 1.f - std::exp(-1.f / (float(refreshRate) * 2.f));
}


LevelMeter::~LevelMeter() = default;

//==============================================================================
void LevelMeter::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds();

    g.fillAll(Colors::LevelMeter::background);
    g.setFont(Fonts::getFont(10.f));

    drawPeakLevel(g, dbLevelL, 0, 7);
    drawPeakLevel(g, dbLevelR, 9, 7);
    drawRmsLevel(g, dbRmsLevelL, 1, 4);
    drawRmsLevel(g, dbRmsLevelR, 10, 4);

    for (float db = maxdB; db >= mindB; db -= stepdB)
    {
        int y = positionForLevel(db);

        g.setColour(Colors::LevelMeter::tickLine);
        g.fillRect(0, y, 16, 1);

        g.setColour(Colors::LevelMeter::tickLabel);
        g.drawSingleLineText(juce::String(int(db)), bounds.getWidth(), y + 3, juce::Justification::right);
    }
}

void LevelMeter::resized()
{
    maxPos = 4.f;
    minPos = float(getHeight()) - 4.f;
}

void LevelMeter::timerCallback()
{
    updateLevel(measurementL.getValue(), levelL, dbLevelL);
    updateLevel(measurementR.getValue(), levelR, dbLevelR);
    dbRmsLevelL = juce::Decibels::gainToDecibels(std::max(rmsMeasurementL.getValue(), clampLevel));
    dbRmsLevelR = juce::Decibels::gainToDecibels(std::max(rmsMeasurementR.getValue(), clampLevel));

    repaint();
}

void LevelMeter::drawPeakLevel(juce::Graphics& g, float level, int x, int width)
{
    drawMeterBar(g, level, x, width, Colors::LevelMeter::peakLevelOK);
}


void LevelMeter::drawRmsLevel(juce::Graphics& g, float level, int x, int width)
{
    drawMeterBar(g, level, x, width, Colors::LevelMeter::rmsLevelOK);
}

void LevelMeter::drawMeterBar(juce::Graphics& g, float levelDB, int x, int width, juce::Colour fillColour)
{
    if (levelDB <= clampdB)
        return;

    const int yStart = positionForLevel(levelDB);
    const int yZero = positionForLevel(0.0f);

    if (yStart < yZero)
    {
        // Above 0 dB — draw overload in red, rest in fill
        g.setColour(Colors::LevelMeter::tooLoud);
        g.fillRect(x, yStart, width, yZero - yStart);

        g.setColour(fillColour);
        g.fillRect(x, yZero, width, getHeight() - yZero);
    }
    else
    {
        g.setColour(fillColour);
        g.fillRect(x, yStart, width, getHeight() - yStart);
    }
}


void LevelMeter::updateLevel(float newLevel, float& smoothedLevel, float& leveldB) const
{
    if (newLevel > smoothedLevel)
    {
        smoothedLevel = newLevel;
    }
    else
    {
        smoothedLevel += (newLevel - smoothedLevel) * decay;
    }

    if (smoothedLevel > clampLevel)
    {
        leveldB = juce::Decibels::gainToDecibels(smoothedLevel);
    }
    else
    {
        leveldB = clampdB;
    }
}

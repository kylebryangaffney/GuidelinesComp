/*
  ==============================================================================

    LevelMeter.h
    Created: 7 Jun 2025 7:39:22am
    Author:  kyleb

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#include "../Service/Measurement.h"
#include "../Service/RMSMeasurement.h"

//==============================================================================
class LevelMeter : public juce::Component,
    private juce::Timer
{
public:
    LevelMeter(const Measurement& measurementL, const Measurement& measurementR, const RmsMeasurement& rmsMeasurementL, const RmsMeasurement& rmsMeasurementR);
    ~LevelMeter() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    const Measurement& measurementL;
    const Measurement& measurementR;
    const RmsMeasurement& rmsMeasurementL;
    const RmsMeasurement& rmsMeasurementR;

    static constexpr float maxdB = 6.f;
    static constexpr float mindB = -60.f;
    static constexpr float stepdB = 6.f;
    static constexpr float clampdB = -120.f;
    static constexpr float clampLevel = 0.000001f;

    static constexpr int refreshRate = 60;

    float decay = 0.f;
    float levelL = clampLevel;
    float levelR = clampLevel;
    float dbLevelL = 0.f;
    float dbLevelR = 0.f;
    float maxPos = 0.f;
    float minPos = 0.f;

    float rmsLevelL = clampLevel;
    float rmsLevelR = clampLevel;
    float dbRmsLevelL = 0.f;
    float dbRmsLevelR = 0.f;

    //==============================================================================
    void timerCallback() override;

    int positionForLevel(float dbLevel) const noexcept
    {
        return int(std::round(juce::jmap(dbLevel, maxdB, mindB, maxPos, minPos)));
    }

    void drawMeterBar(juce::Graphics& g, float levelDB, int x, int width, juce::Colour fillColour);
    void drawPeakLevel(juce::Graphics& g, float level, int x, int width);
    void drawRmsLevel(juce::Graphics& g, float level, int x, int width);
    void updateLevel(float newLevel, float& smoothedLevel, float& leveldB) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeter)
};


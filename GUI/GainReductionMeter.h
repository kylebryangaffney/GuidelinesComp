
/*
  ==============================================================================

    GainReductionMeter.h
    Created: 15 Jun 2025 10:34:12am
    Author:  kyleb

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Service/RMSMeasurement.h"

//==============================================================================
class GainReductionMeter : public juce::Component,
    private juce::Timer
{
public:
    GainReductionMeter(RmsMeasurement& rmsMeasurementL, RmsMeasurement& rmsMeasurementR);
    ~GainReductionMeter() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    RmsMeasurement& rmsMeasurementL;
    RmsMeasurement& rmsMeasurementR;

    static constexpr float maxdB = 0.0f;   // top: no GR
    static constexpr float mindB = 36.0f;  // bottom: max GR
    static constexpr float stepdB = 6.0f;
    static constexpr float clampdB = 0.0f;
    static constexpr float clampLevel = 0.000001f;
    static constexpr int refreshRate = 60;
    static constexpr float maxReduction = 36.0f;

    float decay = 0.f;
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

    void drawRmsLevel(juce::Graphics& g, float level, int x, int width);
    void updateLevel(float newLevel, float& smoothedLevel, float& leveldB) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainReductionMeter)
};


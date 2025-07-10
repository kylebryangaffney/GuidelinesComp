
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

class GainReductionMeter : public juce::Component,
    private juce::Timer
{
public:
    GainReductionMeter(RmsMeasurement& rmsL, RmsMeasurement& rmsR);
    ~GainReductionMeter() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    int positionForLevel(float dbLevel) const noexcept;

    float getMaxRmsLevel() const;
    static constexpr float maxdB = 0.0f;
    static constexpr float mindB = -60.0f;
    static constexpr float stepdB = 6.0f;

private:
    RmsMeasurement& rmsMeasurementL;
    RmsMeasurement& rmsMeasurementR;

    static constexpr float clampdB = mindB;
    static constexpr float clampLevel = 0.000001f;
    static constexpr int refreshRate = 60;

    float decay = 0.f;
    float maxPos = 0.f;
    float minPos = 0.f;

    float rmsLevelL = clampLevel;
    float rmsLevelR = clampLevel;
    float dbRmsLevelL = 0.f;
    float dbRmsLevelR = 0.f;

    void timerCallback() override;
    void updateLevel(float newLevel, float& smoothedLevel, float& leveldB) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainReductionMeter)
};

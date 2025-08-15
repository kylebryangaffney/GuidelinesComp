/*
  ==============================================================================

    GainReductionMeterLAF.h
    Created: 15 Aug 2025 8:30:34am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../GUI/GainReductionMeter.h"
#include "Colors.h"
#include "Fonts.h"
#include "LAFCommon.h"


//==============================================================================
// Gain Reduction Meter Look and Feel
class GainReductionMeterLookAndFeel : public juce::LookAndFeel_V4
{
public:
    GainReductionMeterLookAndFeel() = default;
    static GainReductionMeterLookAndFeel* get()
    {
        static GainReductionMeterLookAndFeel instance;
        return &instance;
    }

    void drawGainReductionMeter(juce::Graphics& g, const GainReductionMeter& meter) noexcept;
    void drawMeterBar(int x, int y, int width, int height, juce::Graphics& g, float levelDB,
        const std::function<int(float)>& positionForLevel, juce::Colour fillColour) noexcept;
    void drawRmsLevel(int x, int y, int width, int height,
        juce::Graphics& g,
        float levelDB,
        const std::function<int(float)>& positionForLevel) noexcept;

private:
    const float clampdB = -120.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainReductionMeterLookAndFeel)
};

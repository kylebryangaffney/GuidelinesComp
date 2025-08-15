/*
  ==============================================================================

    LevelMeterLAF.h
    Created: 15 Aug 2025 8:30:19am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Colors.h"
#include "Fonts.h"
#include "LAFCommon.h"
#include "../GUI/LevelMeter.h"


//==============================================================================
// Level Meter Look and Feel

class LevelMeterLookAndFeel : public juce::LookAndFeel_V4
{
public:
    LevelMeterLookAndFeel() = default;

    static LevelMeterLookAndFeel* get()
    {
        static LevelMeterLookAndFeel instance;
        return &instance;
    }

    void drawLevelMeter(juce::Graphics& g, const LevelMeter& meter) noexcept;

    void drawMeterBar(int x, int y, int width, int height,
        juce::Graphics& g, float levelDB,
        const std::function<int(float)>& positionForLevel,
        juce::Colour fillColour) noexcept;

    void drawPeakLevel(int x, int y, int width, int height,
        juce::Graphics& g,
        float levelDB,
        const std::function<int(float)>& positionForLevel) noexcept;

    void drawRmsLevel(int x, int y, int width, int height,
        juce::Graphics& g,
        float levelDB,
        const std::function<int(float)>& positionForLevel) noexcept;

private:
    static constexpr float clampdB = -120.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeterLookAndFeel)
};

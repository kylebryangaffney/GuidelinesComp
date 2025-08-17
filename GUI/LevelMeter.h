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
/**
    A stereo peak + RMS level meter component.

    This class provides real-time level metering for audio signals, showing both
    peak and RMS values. Peak levels are smoothed using attack/release ballistics
    for responsive but stable movement, while RMS values are fed directly from
    an external `RmsMeasurement`.
*/
class LevelMeter : public juce::Component,
    private juce::Timer
{
public:
    /**
        Constructs a LevelMeter component.

        @param measurementL      Peak measurement buffer for the left channel
        @param measurementR      Peak measurement buffer for the right channel
        @param rmsMeasurementL   RMS measurement reference for the left channel
        @param rmsMeasurementR   RMS measurement reference for the right channel
    */
    LevelMeter(Measurement& measurementL, Measurement& measurementR,
        const RmsMeasurement& rmsMeasurementL, const RmsMeasurement& rmsMeasurementR);

    /// Destructor.
    ~LevelMeter() override;

    /// Maximum displayed dB value (top of the meter).
    static constexpr float maxdB = 6.0f;

    /// Minimum displayed dB value (bottom of the meter).
    static constexpr float mindB = -60.f;

    /// Step size between tick marks in dB.
    static constexpr float stepdB = 6.0f;

    /// Paints the level meter using its LookAndFeel.
    void paint(juce::Graphics&) override;

    /// Handles resize events and updates meter bounds.
    void resized() override;

    /// @returns Smoothed peak level (left channel) in dB.
    [[nodiscard]] float getPeakLevelL() const { return dbLevelL; }

    /// @returns Smoothed peak level (right channel) in dB.
    [[nodiscard]] float getPeakLevelR() const { return dbLevelR; }

    /// @returns RMS level (left channel) in dB.
    [[nodiscard]] float getRmsLevelL()  const { return dbRmsLevelL; }

    /// @returns RMS level (right channel) in dB.
    [[nodiscard]] float getRmsLevelR()  const { return dbRmsLevelR; }

    /**
        Maps a dB value into a horizontal pixel position.

        @param dbLevel  The input level in dB
        @param minX     Minimum X position (left bound)
        @param maxX     Maximum X position (right bound)
        @returns Pixel position corresponding to dbLevel
    */
    [[nodiscard]] int positionForLevel(const float dbLevel,
        const float minX,
        const float maxX) const noexcept;

private:
    //==============================================================================
    Measurement& measurementL;       ///< Peak measurement buffer (left)
    Measurement& measurementR;       ///< Peak measurement buffer (right)
    const RmsMeasurement& rmsMeasurementL; ///< RMS measurement reference (left)
    const RmsMeasurement& rmsMeasurementR; ///< RMS measurement reference (right)

    static constexpr float clampdB = -120.f;     ///< Lower clamp in dB
    static constexpr float clampLevel = 0.000001f;  ///< Linear floor for gain values
    static constexpr int  refreshRate = 60;         ///< Refresh rate in Hz

    float dbLevelL = clampdB;   ///< Smoothed peak level (dB, left)
    float dbLevelR = clampdB;   ///< Smoothed peak level (dB, right)
    float dbRmsLevelL = clampdB;   ///< RMS level (dB, left)
    float dbRmsLevelR = clampdB;   ///< RMS level (dB, right)
    float levelL = clampLevel;///< Smoothed peak level (linear, left)
    float levelR = clampLevel;///< Smoothed peak level (linear, right)
    float rmsLevelL = clampLevel;///< RMS level (linear, left)
    float rmsLevelR = clampLevel;///< RMS level (linear, right)

    float maxPos = 0.f; ///< Rightmost meter pixel
    float minPos = 0.f; ///< Leftmost meter pixel

    double lastUpdateMs = 0.0;  ///< Last GUI update timestamp (ms)
    double attackT = 0.02; ///< Attack time constant (s)
    double releaseT = 0.80; ///< Release time constant (s)

    //==============================================================================
    /// Called at the refresh rate to update meter values.
    void timerCallback() override;

    /**
        Updates and smooths a single channel's peak level.

        @param newLevel       The new input linear peak level
        @param smoothedLevel  Reference to smoothed linear peak level (modified)
        @param leveldB        Reference to smoothed peak level in dB (modified)
        @param dtSec          Elapsed time since last update in seconds
    */
    void updateLevel(float newLevel, float& smoothedLevel,
        float& leveldB, float dtSec) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeter)
};

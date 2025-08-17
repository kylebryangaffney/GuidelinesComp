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

/**
 * @class GainReductionMeter
 * @brief A GUI component that displays gain reduction levels with
 *        adjustable attack and release ballistics.
 *
 * Reads left/right RMS measurements from the audio thread, smooths them
 * using dt-aware attack/release filters, and repaints at a fixed GUI
 * refresh rate.
 */
class GainReductionMeter : public juce::Component,
    private juce::Timer
{
public:
    /**
     * @brief Construct a new GainReductionMeter.
     * @param rmsL Reference to the left channel RMS measurement.
     * @param rmsR Reference to the right channel RMS measurement.
     */
    GainReductionMeter(const RmsMeasurement& rmsL, const RmsMeasurement& rmsR);

    /** Destructor. */
    ~GainReductionMeter() override;

    /** Paints the meter using the assigned LookAndFeel. */
    void paint(juce::Graphics&) override;

    /** Updates layout bounds when the component is resized. */
    void resized() override;

    /**
     * @brief Map a dB value into a pixel coordinate.
     * @param dbLevel Input level in dB.
     * @param minX Minimum pixel coordinate.
     * @param maxX Maximum pixel coordinate.
     * @return Pixel position corresponding to dbLevel.
     */
    int positionForLevel(float dbLevel, float minX, float maxX) const noexcept;

    /**
     * @brief Returns the higher of the left or right channel dB levels.
     * @return Maximum dB value between left and right.
     */
    float getMaxRmsLevel() const;

    /// Maximum displayed dB (no gain reduction).
    static constexpr float maxdB = 0.0f;
    /// Minimum displayed dB (max gain reduction).
    static constexpr float mindB = -30.0f;
    /// Step size in dB for ticks.
    static constexpr float stepdB = 3.0f;

private:
    const RmsMeasurement& rmsMeasurementL;  ///< Left channel RMS source
    const RmsMeasurement& rmsMeasurementR;  ///< Right channel RMS source

    static constexpr float clampdB = mindB;
    static constexpr float clampLevel = 0.000001f;
    static constexpr int refreshRate = 60; ///< GUI update frequency (Hz)

    float maxPos = 0.0f;
    float minPos = 0.0f;

    float rmsLevelL = mindB;
    float rmsLevelR = mindB;
    float dbRmsLevelL = 0.0f;
    float dbRmsLevelR = 0.0f;

    double lastUpdateMs = 0.0;   ///< Last GUI update timestamp (ms)
    double attackT = 0.02;       ///< Attack time constant (s)
    double releaseT = 0.80;      ///< Release time constant (s)

    /** Periodic GUI update. Reads RMS values, smooths, and repaints. */
    void timerCallback() override;

    /**
     * @brief Smooth an input dB value toward a target with attack/release ballistics.
     * @param newLevel Input level in dB.
     * @param smoothedLevel Reference to smoothed value.
     * @param leveldB Reference to published dB level.
     * @param dtSec Elapsed time since last update (s).
     */
    void updateLevel(float newLevel, float& smoothedLevel, float& leveldB, const float dtSec) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainReductionMeter)
};

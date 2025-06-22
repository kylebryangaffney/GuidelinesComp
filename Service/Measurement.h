/*
  ==============================================================================

    Measurement.h
    Created: 8 Mar 2025 9:46:35am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <atomic>

//==============================================================================
struct Measurement
{
    void reset() noexcept
    {
        peak.store(0.f);
    }

    void updateIfGreater(float newValue) noexcept
    {
        auto oldValue = peak.load();
        while (newValue > oldValue && !peak.compare_exchange_weak(oldValue, newValue));
    }

    void updateSmoothed() {
        smoothed.setTargetValue(peak.load());
    }

    float getSmoothed() {
        return smoothed.getNextValue();
    }

    float readAndReset() noexcept
    {
        return peak.exchange(0.f);
    }

    float getPeak() const noexcept
    {
        return peak.load();
    }

    void prepare(double sampleRate, double smoothingTimeSeconds)
    {
        smoothed.reset(sampleRate, smoothingTimeSeconds);
        smoothed.setCurrentAndTargetValue(0.0f);
    }

private:
    std::atomic<float> peak{ 0.0f };
    juce::LinearSmoothedValue<float> smoothed{ 0.0f };
};
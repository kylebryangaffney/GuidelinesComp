/*
  ==============================================================================

    Measurement.h
    Created: 8 Mar 2025 9:46:35am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <atomic>
#include <JuceHeader.h> // for LinearSmoothedValue

/**
    Thread-safe peak level tracker with smoothed value support.
    This struct is designed to be updated from the audio thread using atomic operations,
    and read from the GUI thread using a smoothed approximation of the peak value.
*/
struct Measurement
{
    /**
        Resets the peak value to 0.
        Typically called when initializing or clearing the measurement state.
    */
    void reset() noexcept
    {
        peak.store(0.f);
    }

    /**
        Atomically updates the peak if the provided value is greater than the current peak.
        This function is real-time safe and non-blocking.
        @param newValue The new value to compare against the current peak.
    */
    void updateIfGreater(float newValue) noexcept
    {
        float oldValue = peak.load();
        while (newValue > oldValue && !peak.compare_exchange_weak(oldValue, newValue));
    }

    /**
        Sets the smoothing target to the current peak value.

        This should be called periodically on a non-audio thread (e.g., GUI thread).
    */
    void updateSmoothed()
    {
        smoothed.setTargetValue(peak.load());
    }

    /**
        Returns the next smoothed value.

        Call this regularly on the GUI thread to produce smoothly animated output.

        @return The next interpolated smoothed value.
    */
    float getSmoothed()
    {
        return smoothed.getNextValue();
    }

    /**
        Atomically reads and resets the peak value to 0.
        Useful for peak-hold behavior that clears after each read.
        @return The value of the peak before it was reset.
    */
    float readAndReset() noexcept
    {
        return peak.exchange(0.f);
    }

    /**
        Returns the current raw peak value without resetting it.
        This can be used when you want instantaneous access to the latest value.
        @return The current peak value.
    */
    float getPeak() const noexcept
    {
        return peak.load();
    }

    /**
        Prepares the smoothed value with the given sample rate and smoothing time.
        This must be called before using the smoothing functions.
        @param sampleRate The host or processing sample rate in Hz.
        @param smoothingTimeSeconds The time constant for smoothing (in seconds).
    */
    void prepare(double sampleRate, double smoothingTimeSeconds)
    {
        smoothed.reset(sampleRate, smoothingTimeSeconds);
        smoothed.setCurrentAndTargetValue(0.0f);
    }

private:
    std::atomic<float> peak{ 0.0f };                         ///< Atomically updated peak value
    juce::LinearSmoothedValue<float> smoothed{ 0.0f };       ///< Smoothed value for UI display
};

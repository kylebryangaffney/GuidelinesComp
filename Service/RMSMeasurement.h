/*
  ==============================================================================

    RMSMeasurement.h
    Created: 7 Jun 2025 7:57:35am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <atomic>
#include <cmath>

/**
    Thread-safe RMS (Root Mean Square) level tracker for real-time audio.
    This class accumulates squared samples in a lock-free way and computes the
    RMS value over a given window. Call `update()` on each sample, and then
    call `computeRMS()` at regular intervals (e.g. once per audio block).
*/
struct RmsMeasurement
{
    /**
        Resets the internal state, clearing sum of squares, sample count, and stored RMS.
    */
    void reset() noexcept
    {
        sumSquares.store(0.f);
        numSamples.store(0);
        value.store(0.f);
    }

    /**
        Accumulates a squared sample value.
        This is designed to be called for each audio sample in a block.
        @param sample The audio sample to include in the RMS calculation.
    */
    void update(float sample) noexcept
    {
        sumSquares.fetch_add(sample * sample);
        numSamples.fetch_add(1);
    }

    /**
        Computes the RMS value from the accumulated data.
        To be called once per block (or fixed interval).
        Resets internal accumulators after computing.
        Result is stored in `value` and can be retrieved using `getValue()`.
    */
    void computeRMS() noexcept
    {
        float sum = sumSquares.exchange(0.f);
        int count = numSamples.exchange(0);
        float rms = (count > 0) ? std::sqrt(sum / count) : 0.0f;
        value.store(rms);
    }

    /**
        Returns the most recently computed RMS value.
        @return The stored RMS result (linear, not in dB).
    */
    float getValue() const noexcept
    {
        return value.load();
    }

private:
    std::atomic<float> sumSquares{ 0.f }; ///< Accumulator for squared samples
    std::atomic<int> numSamples{ 0 };  ///< Count of samples accumulated
    std::atomic<float> value{ 0.f };///< Most recent RMS result
};

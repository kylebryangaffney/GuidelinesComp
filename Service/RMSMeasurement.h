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

struct RmsMeasurement
{
    void reset() noexcept
    {
        sumSquares.store(0.f);
        numSamples.store(0);
    }

    void update(float sample) noexcept
    {
        float square = sample * sample;
        sumSquares.fetch_add(square);
        numSamples.fetch_add(1);
    }

    float readAndReset() noexcept
    {
        float sum = sumSquares.exchange(0.f);
        int count = numSamples.exchange(0);
        float rms = std::sqrt(sum / count);
        return (count > 0) ? rms : 0.0f;
    }

private:
    std::atomic<float> sumSquares{ 0.f };
    std::atomic<int> numSamples{ 0 };
};

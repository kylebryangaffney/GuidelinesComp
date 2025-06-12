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
        value.store(0.f);
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
        float rms = (count > 0) ? std::sqrt(sum / count) : 0.0f;
        value.store(rms);
        DBG("RMS: " << rms << " | Count: " << count);

        return rms;
    }

    float getValue() const noexcept
    {
        return value.load();
    }
    

private:
    std::atomic<float> sumSquares{ 0.f };
    std::atomic<int> numSamples{ 0 };
    std::atomic<float> value{ 0.0f };
};
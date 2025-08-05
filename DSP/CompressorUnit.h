/*
  ==============================================================================

    CompressorUnit.h
    Created: 27 May 2025 8:30:48am
    Author:  kyleb

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>

/**
    A basic VCA-style compressor unit controlled via attack, release, threshold, and ratio parameters.
    This class encapsulates a JUCE `dsp::Compressor` and adds smoothed parameter updates.
    It is typically controlled using mapped values from a UI control scheme such as "control" and "compress" knobs.
*/
class CompressorUnit
{
public:
    /// Constructs a default compressor unit.
    CompressorUnit() = default;

    /**
        Prepares the compressor for playback.
        Sets the internal sample rate and initializes smoothed parameters.
        param spec A JUCE `ProcessSpec` containing sample rate, block size, and number of channels.
    */
    void prepare(const juce::dsp::ProcessSpec& spec);

    /**
        Resets the internal state of the compressor.
        Typically called when playback is stopped or reinitialized.
    */
    void reset();

    /**
        Updates compressor parameters with smoothing.
        This sets the target values for the internal `LinearSmoothedValue` objects, which are used to avoid audio artifacts.
        param thresholdDb  The threshold in decibels.
        param attackMs     The attack time in milliseconds (must be ≥ 0).
        param releaseMs    The release time in milliseconds (must be ≥ 0).
        param ratioVal     The compression ratio (must be ≥ 1.0).
    */
    void updateCompressorSettings(const float attackMs,
        const float releaseMs,
        const float ratioVal,
        const float thresholdDb);

    /**
        Applies compression to the given audio buffer.
        This retrieves the next smoothed values for each parameter and applies the compressor to the audio block.
        @param context A JUCE `ProcessContextReplacing` object representing the audio block to process.
    */
    void processCompression(juce::dsp::ProcessContextReplacing<float>& context);

private:
    juce::dsp::Compressor<float> compressor;

    const double smoothingInSeconds = 0.01; ///< Smoothing time constant for parameter transitions.

    juce::LinearSmoothedValue<float> attackSmoothed;
    juce::LinearSmoothedValue<float> releaseSmoothed;
    juce::LinearSmoothedValue<float> ratioSmoothed;
    juce::LinearSmoothedValue<float> thresholdSmoothed;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorUnit)
};

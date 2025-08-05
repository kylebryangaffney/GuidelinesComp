/*
  ==============================================================================

    OptoCompressorUnit.h
    Created: 29 May 2025 8:51:19am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/**
    A simple opto-style compressor that mimics analog optical compression behavior
    using block RMS detection and a smoothed gain envelope.
    Fixed parameters (attack, release, ratio, threshold) define compression character.
*/
class OptoCompressorUnit
{
public:
    /// Constructs a default OptoCompressorUnit.
    OptoCompressorUnit() = default;

    /**
        Prepares the compressor for playback with a given processing specification.
        @param spec The JUCE DSP process specification (sample rate, block size, channels).
    */
    void prepare(const juce::dsp::ProcessSpec& spec);

    /**
        Resets the internal state of the compressor, including gain, smoothing, and envelope.
    */
    void reset();

    /**
        Processes a block of audio using opto-style compression.
        Applies envelope following, computes gain reduction, smooths gain,
        and applies the final gain to the audio block.
        @param context A JUCE processing context containing the audio block.
    */
    void processCompression(juce::dsp::ProcessContextReplacing<float> context);

private:
    juce::dsp::Gain<float> optoGain;                   ///< Gain processor for applying gain reduction
    juce::LinearSmoothedValue<float> smoothedGain;     ///< Smoother to prevent gain stepping artifacts

    const double optoSmoothingTime = 0.1;              ///< Smoothing time for output gain, in seconds
    const int smoothingSteps = 4;                      ///< Number of times to sample smoothing per block

    float fixedAttack = 15.0f;                         ///< Fixed attack time in ms
    float fixedRelease = 120.0f;                       ///< Fixed release time in ms
    float fixedRatio = 5.0f;                           ///< Compression ratio
    float fixedThreshold = -18.0f;                     ///< Compression threshold in dB

    double sampleRate = 44100.0;                       ///< Current sample rate
    float envelopeDb = -100.0f;                        ///< Smoothed input level in dB

    float attackCoeff = 0.0f;                          ///< Coefficient for attack smoothing
    float releaseCoeff = 0.0f;                         ///< Coefficient for release smoothing

    /**
        Computes the RMS level of a block and returns it in decibels.
        @param block A block of audio samples.
        @return The RMS level in dBFS, floor-limited to -100 dB.
    */
    float calculateRMS(juce::dsp::AudioBlock<float> block);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OptoCompressorUnit)
};

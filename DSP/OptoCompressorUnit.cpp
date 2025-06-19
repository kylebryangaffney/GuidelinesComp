/*
  ==============================================================================

    OptoCompressorUnit.cpp
    Created: 29 May 2025 8:51:19am
    Author:  kyleb

  ==============================================================================
*/
#include "OptoCompressorUnit.h"

//==============================================================================
void OptoCompressorUnit::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    // Convert fixed attack/release times from ms to seconds
    const double attackTimeSec = fixedAttack / 1000.0;
    const double releaseTimeSec = fixedRelease / 1000.0;

    // Compute smoothing coefficients for envelope filter
    attackCoeff = std::exp(-1.0 / (sampleRate * attackTimeSec));
    releaseCoeff = std::exp(-1.0 / (sampleRate * releaseTimeSec));

    // Initialize envelope follower level very low
    envelopeDb = -100.0f;

    // Prepare the gain processor for gain reduction
    optoGain.prepare(spec);
    optoGain.setGainDecibels(0.0f);

    // Setup smoothed value for final gain stage to avoid zipper noise
    smoothedGain.reset(sampleRate, optoSmoothingTime);
    smoothedGain.setCurrentAndTargetValue(1.0f);
}

//==============================================================================
// Reset internal state for fresh processing (e.g., when transport stops)
void OptoCompressorUnit::reset()
{
    optoGain.reset();
    smoothedGain.reset(sampleRate, optoSmoothingTime);
    smoothedGain.setCurrentAndTargetValue(1.0f);

    envelopeDb = -100.0f;
}

//==============================================================================
// Main compression routine: follow input envelope, compute gain reduction, apply gain
void OptoCompressorUnit::processCompression(juce::dsp::ProcessContextReplacing<float> context)
{
    const juce::dsp::AudioBlock<float>& block = context.getOutputBlock();

    // Calculate block RMS and convert to dB
    float inputLevelDb = calculateRMS(block);

    // Update envelope: attack when rising, release when falling
    if (inputLevelDb > envelopeDb)
        envelopeDb += (inputLevelDb - envelopeDb) * attackCoeff;
    else
        envelopeDb += (inputLevelDb - envelopeDb) * releaseCoeff;

    // Compute how much signal exceeds threshold
    float overshootDb = envelopeDb - fixedThreshold;

    // Calculate gain reduction in dB using simple linear ratio law
    float gainReductionDb = (overshootDb > 0.0f)
        ? overshootDb * (1.0f - 1.0f / fixedRatio)
        : 0.0f;

    // Convert reduction to linear gain (negative dB means reduction)
    float linearGain = juce::Decibels::decibelsToGain(-gainReductionDb);

    // Smooth gain changes to avoid clicks
    smoothedGain.setTargetValue(linearGain);
    optoGain.setGainLinear(smoothedGain.getNextValue());

    // Apply gain reduction to audio
    optoGain.process(context);
}

//==============================================================================
// Utility: block-wise RMS calculation, returns level in dB
float OptoCompressorUnit::calculateRMS(juce::dsp::AudioBlock<float> block)
{
    int numChannels = static_cast<int>(block.getNumChannels());
    int numSamples = static_cast<int>(block.getNumSamples());

    float sumOfSquares = 0.0f;

    for (int chan = 0; chan < numChannels; ++chan)
    {
        for (int samp = 0; samp < numSamples; ++samp)
        {
            float sample = block.getSample(chan, samp);
            sumOfSquares += sample * sample;
        }
    }

    float meanSquare = sumOfSquares / (numChannels * numSamples);
    float rms = std::sqrt(meanSquare);

    // Convert to dBFS, with a floor to avoid log(0)
    return juce::Decibels::gainToDecibels(rms, -100.0f);
}

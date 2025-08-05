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

    // Convert attack/release times to seconds for smoothing coefficient calculation
    const double attackTimeSec = fixedAttack / 1000.0;
    const double releaseTimeSec = fixedRelease / 1000.0;

    // Compute one-pole smoothing coefficients for envelope detection
    attackCoeff = std::exp(-1.0 / (sampleRate * attackTimeSec));
    releaseCoeff = std::exp(-1.0 / (sampleRate * releaseTimeSec));

    // Initialize the envelope follower level
    envelopeDb = -100.0f;

    // Prepare the gain processor (applies gain reduction)
    optoGain.prepare(spec);
    optoGain.setGainDecibels(0.0f);

    // Set up smoothing for the output gain to avoid zipper noise
    smoothedGain.reset(sampleRate, optoSmoothingTime);
    smoothedGain.setCurrentAndTargetValue(1.0f);
}

//==============================================================================
void OptoCompressorUnit::reset()
{
    // Reset gain and smoother state
    optoGain.reset();
    smoothedGain.reset(sampleRate, optoSmoothingTime);
    smoothedGain.setCurrentAndTargetValue(1.0f);

    // Reset envelope follower
    envelopeDb = -100.0f;
}

//==============================================================================
void OptoCompressorUnit::processCompression(juce::dsp::ProcessContextReplacing<float> context)
{
    const juce::dsp::AudioBlock<float>& block = context.getOutputBlock();

    // Measure block RMS and convert to dB
    float inputLevelDb = calculateRMS(block);

    // Envelope follower with separate attack/release smoothing
    if (inputLevelDb > envelopeDb)
        envelopeDb += (inputLevelDb - envelopeDb) * attackCoeff;
    else
        envelopeDb += (inputLevelDb - envelopeDb) * releaseCoeff;

    // Compute overshoot above threshold
    float overshootDb = envelopeDb - fixedThreshold;

    // Apply ratio if overshoot exists
    float gainReductionDb = (overshootDb > 0.0f)
        ? overshootDb * (1.0f - 1.0f / fixedRatio)
        : 0.0f;

    // Convert gain reduction to linear (negative dB = attenuation)
    float linearGain = juce::Decibels::decibelsToGain(-gainReductionDb);

    // Push new gain reduction target to the smoother
    smoothedGain.setTargetValue(linearGain);

    // Step through smoothing curve to approximate behavior across block
    float gainSum = 0.0f;
    for (int i = 0; i < smoothingSteps; ++i)
        gainSum += smoothedGain.getNextValue();

    const float averagedGain = gainSum / static_cast<float>(smoothingSteps);

    // Apply smoothed gain to audio
    optoGain.setGainLinear(averagedGain);
    optoGain.process(context);
}

//==============================================================================
float OptoCompressorUnit::calculateRMS(juce::dsp::AudioBlock<float> block)
{
    int numChannels = static_cast<int>(block.getNumChannels());
    int numSamples = static_cast<int>(block.getNumSamples());

    float sumOfSquares = 0.0f;

    // Accumulate square of all samples
    for (int chan = 0; chan < numChannels; ++chan)
    {
        for (int samp = 0; samp < numSamples; ++samp)
        {
            float sample = block.getSample(chan, samp);
            sumOfSquares += sample * sample;
        }
    }

    // Calculate RMS from mean square
    float meanSquare = sumOfSquares / (numChannels * numSamples);
    float rms = std::sqrt(meanSquare);

    // Convert to dBFS with a floor to avoid log(0)
    return juce::Decibels::gainToDecibels(rms, -100.0f);
}

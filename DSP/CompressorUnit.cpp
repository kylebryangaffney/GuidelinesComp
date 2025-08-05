/*
  ==============================================================================

    CompressorUnit.cpp
    Created: 27 May 2025 8:30:48am
    Author:  kyleb

  ==============================================================================
*/


#include "CompressorUnit.h"

void CompressorUnit::prepare(const juce::dsp::ProcessSpec& spec)
{
    // Prepare the internal JUCE compressor with the given spec
    compressor.prepare(spec);

    // Set up smoothing for all parameters
    attackSmoothed.reset(spec.sampleRate, smoothingInSeconds);
    releaseSmoothed.reset(spec.sampleRate, smoothingInSeconds);
    ratioSmoothed.reset(spec.sampleRate, smoothingInSeconds);
    thresholdSmoothed.reset(spec.sampleRate, smoothingInSeconds);

    // Set default parameter values
    attackSmoothed.setCurrentAndTargetValue(50.f);      // ms
    releaseSmoothed.setCurrentAndTargetValue(55.0f);     // ms
    ratioSmoothed.setCurrentAndTargetValue(2.0f);        // 2:1 ratio
    thresholdSmoothed.setCurrentAndTargetValue(-12.0f);  // dB
}

void CompressorUnit::reset()
{
    compressor.reset();
}


void CompressorUnit::updateCompressorSettings(const float attackMs,
    const float releaseMs,
    const float ratioVal,
    const float thresholdDb)
{
    // Validate inputs to prevent runtime issues
    jassert(attackMs >= 0.0f && "Attack must be >= 0 ms");
    jassert(releaseMs >= 0.0f && "Release must be >= 0 ms");
    jassert(ratioVal >= 1.0f && "Ratio must be >= 1.0");

    // Apply new smoothed target values
    attackSmoothed.setTargetValue(attackMs);
    releaseSmoothed.setTargetValue(releaseMs);
    ratioSmoothed.setTargetValue(ratioVal);
    thresholdSmoothed.setTargetValue(thresholdDb);
}

void CompressorUnit::processCompression(juce::dsp::ProcessContextReplacing<float>& context)
{
    // Apply smoothed values to the compressor
    compressor.setAttack(attackSmoothed.getNextValue());
    compressor.setRelease(releaseSmoothed.getNextValue());
    compressor.setRatio(ratioSmoothed.getNextValue());
    compressor.setThreshold(thresholdSmoothed.getNextValue());

    // Process the incoming audio block
    compressor.process(context);
}

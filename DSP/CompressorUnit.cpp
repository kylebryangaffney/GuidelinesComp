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
    compressor.prepare(spec);

    attackSmoothed.reset(spec.sampleRate, smootheningInSeconds);
    releaseSmoothed.reset(spec.sampleRate, smootheningInSeconds);
    ratioSmoothed.reset(spec.sampleRate, smootheningInSeconds);
    thresholdSmoothed.reset(spec.sampleRate, smootheningInSeconds);

    attackSmoothed.setCurrentAndTargetValue( 50.f);
    releaseSmoothed.setCurrentAndTargetValue( 55.0f);
    ratioSmoothed.setCurrentAndTargetValue( 2.0f);
    thresholdSmoothed.setCurrentAndTargetValue( -12.0f);
}

void CompressorUnit::reset()
{
    compressor.reset();
}

void CompressorUnit::updateCompressorSettings(float attackMs, float releaseMs, float ratioVal, float thresholdDb)
{
    jassert(attackMs && releaseMs && ratioVal && thresholdDb);

    attackSmoothed.setTargetValue(attackMs);
    releaseSmoothed.setTargetValue(releaseMs);
    ratioSmoothed.setTargetValue(ratioVal);
    thresholdSmoothed.setTargetValue(thresholdDb);
}

void CompressorUnit::processCompression(juce::dsp::AudioBlock<float> block)
{
    juce::dsp::ProcessContextReplacing<float> ctx(block);

    compressor.setAttack(attackSmoothed.getNextValue());
    compressor.setRelease(releaseSmoothed.getNextValue());
    compressor.setRatio(ratioSmoothed.getNextValue());
    compressor.setThreshold(thresholdSmoothed.getNextValue());

    compressor.process(ctx);
}

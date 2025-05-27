/*
  ==============================================================================

    CompressorUnit.cpp
    Created: 27 May 2025 8:30:48am
    Author:  kyleb

  ==============================================================================
*/

#include "CompressorUnit.h"

CompressorUnit::CompressorUnit(juce::AudioParameterFloat* attackParam,
    juce::AudioParameterFloat* releaseParam,
    juce::AudioParameterFloat* ratioParam,
    juce::AudioParameterFloat* thresholdParam)
    : attack(attackParam),
    release(releaseParam),
    ratio(ratioParam),
    threshold(thresholdParam)
{
}

void CompressorUnit::configure(juce::AudioParameterFloat* attackParam,
    juce::AudioParameterFloat* releaseParam,
    juce::AudioParameterFloat* ratioParam,
    juce::AudioParameterFloat* thresholdParam)
{
    jassert(attack && release && ratio && threshold);

    attack = attackParam;
    release = releaseParam;
    ratio = ratioParam;
    threshold = thresholdParam;
}


void CompressorUnit::prepare(const juce::dsp::ProcessSpec& spec)
{
    const double smootheningInSeconds = 0.0002;
    compressor.prepare(spec);

    attackSmoothed.reset(spec.sampleRate, smootheningInSeconds);
    releaseSmoothed.reset(spec.sampleRate, smootheningInSeconds);
    ratioSmoothed.reset(spec.sampleRate, smootheningInSeconds);
    thresholdSmoothed.reset(spec.sampleRate, smootheningInSeconds);

    attackSmoothed.setCurrentAndTargetValue(attack ? attack->get() : 50.f);
    releaseSmoothed.setCurrentAndTargetValue(release ? release->get() : 55.0f);
    ratioSmoothed.setCurrentAndTargetValue(ratio ? ratio->get() : 2.0f);
    thresholdSmoothed.setCurrentAndTargetValue(threshold ? threshold->get() : -12.0f);
}

void CompressorUnit::reset()
{
    compressor.reset();
}

void CompressorUnit::updateCompressorSettings()
{
    jassert(attack && release && ratio && threshold);

    compressor.setAttack(attack->get());
    compressor.setRelease(release->get());
    compressor.setRatio(ratio->get());
    compressor.setThreshold(threshold->get());
}

void CompressorUnit::processCompression(juce::dsp::ProcessContextReplacing<float>& context)
{
    compressor.setAttack(attackSmoothed.getNextValue());
    compressor.setRelease(releaseSmoothed.getNextValue());
    compressor.setRatio(ratioSmoothed.getNextValue());
    compressor.setThreshold(thresholdSmoothed.getNextValue());

    compressor.process(context);
}

/*
  ==============================================================================

    OptoCompressorUnit.cpp
    Created: 29 May 2025 8:51:19am
    Author:  kyleb

  ==============================================================================
*/

#include "OptoCompressorUnit.h"

void OptoCompressorUnit::prepare(const juce::dsp::ProcessSpec& spec)
{
    optoGain.prepare(spec);
    optoGain.setGainDecibels(0.0f);

    attackSmoothed.reset(spec.sampleRate, optoSmoothingTime);
    releaseSmoothed.reset(spec.sampleRate, optoSmoothingTime);
    ratioSmoothed.reset(spec.sampleRate, optoSmoothingTime);
    thresholdSmoothed.reset(spec.sampleRate, optoSmoothingTime);
    smoothedGain.reset(spec.sampleRate, optoSmoothingTime);

    attackSmoothed.setCurrentAndTargetValue(fixedAttack);
    releaseSmoothed.setCurrentAndTargetValue(fixedRelease);
    ratioSmoothed.setCurrentAndTargetValue(fixedRatio);
    thresholdSmoothed.setCurrentAndTargetValue(fixedThreshold);
    smoothedGain.setCurrentAndTargetValue(1.0f);
}

void OptoCompressorUnit::reset()
{
    optoGain.reset();
    attackSmoothed.reset(optoSmoothingTime);
    releaseSmoothed.reset(optoSmoothingTime);
    ratioSmoothed.reset(optoSmoothingTime);
    thresholdSmoothed.reset(optoSmoothingTime);
}


void OptoCompressorUnit::processCompression(juce::dsp::AudioBlock<float> block)
{
    juce::dsp::ProcessContextReplacing<float> context = juce::dsp::ProcessContextReplacing<float>(block);

    float inputLevelDb = calculatePeakOrRMS(block);

    float overshootDb = inputLevelDb - thresholdSmoothed.getNextValue();
    float gainReductionDb = (overshootDb > 0.0f) ? overshootDb * (1.0f - 1.0f / ratioSmoothed.getNextValue()) : 0.0f;
    float linearGain = juce::Decibels::decibelsToGain(-gainReductionDb);

    smoothedGain.setTargetValue(linearGain);
    optoGain.setGainLinear(smoothedGain.getNextValue());
    optoGain.process(context);
}

float OptoCompressorUnit::calculatePeakOrRMS(juce::dsp::AudioBlock<float> block)
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

    return juce::Decibels::gainToDecibels(rms, -100.0f);
}

/*
  ==============================================================================

    OptoCompressorUnit.h
    Created: 29 May 2025 8:51:19am
    Author:  kyleb

  ==============================================================================
*/


#pragma once

#include <JuceHeader.h>


class OptoCompressorUnit
{
public:
    OptoCompressorUnit() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void processCompression(juce::dsp::ProcessContextReplacing<float> context);

private:
private:
    juce::dsp::Gain<float> optoGain;
    juce::LinearSmoothedValue<float> smoothedGain;

    const double optoSmoothingTime = 0.1;

    float fixedAttack = 15.0f;
    float fixedRelease = 120.0f;
    float fixedRatio = 5.0f;
    float fixedThreshold = -18.0f;

    double sampleRate = 44100.0;
    float envelopeDb = -100.0f;

    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;

    float calculateRMS(juce::dsp::AudioBlock<float> block);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OptoCompressorUnit)

};

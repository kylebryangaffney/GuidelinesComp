/*
  ==============================================================================

    CompressorUnit.h
    Created: 27 May 2025 8:30:48am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


class CompressorUnit
{
public:
    CompressorUnit() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void updateCompressorSettings(float attackMs, float releaseMs, float ratioVal, float thresholdDb);
    void processCompression(juce::dsp::ProcessContextReplacing<float>& context);

private:
    juce::dsp::Compressor<float> compressor;

    juce::LinearSmoothedValue<float> attackSmoothed;
    juce::LinearSmoothedValue<float> releaseSmoothed;
    juce::LinearSmoothedValue<float> ratioSmoothed;
    juce::LinearSmoothedValue<float> thresholdSmoothed;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorUnit)

};
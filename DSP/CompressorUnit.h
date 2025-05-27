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
    CompressorUnit(juce::AudioParameterFloat* attackParam,
        juce::AudioParameterFloat* releaseParam,
        juce::AudioParameterFloat* ratioParam,
        juce::AudioParameterFloat* thresholdParam);

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void updateCompressorSettings();
    void processCompression(juce::dsp::ProcessContextReplacing<float>& context);
    void configure(juce::AudioParameterFloat* attackParam,
        juce::AudioParameterFloat* releaseParam,
        juce::AudioParameterFloat* ratioParam,
        juce::AudioParameterFloat* thresholdParam);

private:
    juce::dsp::Compressor<float> compressor;

    juce::AudioParameterFloat* attack;
    juce::AudioParameterFloat* release;
    juce::AudioParameterFloat* ratio;
    juce::AudioParameterFloat* threshold;

    juce::LinearSmoothedValue<float> attackSmoothed;
    juce::LinearSmoothedValue<float> releaseSmoothed;
    juce::LinearSmoothedValue<float> ratioSmoothed;
    juce::LinearSmoothedValue<float> thresholdSmoothed;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorUnit)

};
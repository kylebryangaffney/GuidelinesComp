/*
  ==============================================================================

    RotaryKnob.h
    Created: 4 Jun 2025 8:30:46am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class RotaryKnob : public juce::Component
{
public:
    RotaryKnob(const juce::String& text,
        juce::AudioProcessorValueTreeState& apvts,
        const juce::ParameterID& parameterID,
        bool drawFromMiddle = false);

    ~RotaryKnob() override;

    void resized() override;

    juce::Slider slider;
    juce::Label label;

    void setAlertLevel(float newAlert);
    float getAlertLevel() const;
    virtual int getNumTicks() const { return 21; }


private:
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;

    float alertLevel = 0.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryKnob)
};

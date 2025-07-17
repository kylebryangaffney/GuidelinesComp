/*
  ==============================================================================

    RotaryKnob.cpp
    Created: 4 Jun 2025 8:30:46am
    Author:  kyleb

  ==============================================================================
*/

#include <JuceHeader.h>
#include "RotaryKnob.h"
#include "LookAndFeel.h"

//==============================================================================
RotaryKnob::RotaryKnob(const juce::String& text,
    juce::AudioProcessorValueTreeState& apvts,
    const juce::ParameterID& parameterID,
    bool drawFromMiddle)
    : attachment(apvts, parameterID.getParamID(), slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 57, 13);
    slider.setBounds(0, 0, 57, 68);
    slider.setRotaryParameters(1.25f * juce::MathConstants<float>::pi,
        2.75f * juce::MathConstants<float>::pi,
        true);
    slider.getProperties().set("drawFromMiddle", drawFromMiddle);
    addAndMakeVisible(slider);


    label.setText(text, juce::NotificationType::dontSendNotification);
    label.setJustificationType(juce::Justification::horizontallyCentred);
    label.setBorderSize(juce::BorderSize<int>{ 0, 0, 2, 0 });
    label.attachToComponent(&slider, false);
    addAndMakeVisible(label);

    setLookAndFeel(RotaryKnobLookAndFeel::get());
    setSize(57, 100);
}

RotaryKnob::~RotaryKnob()
{
}

//==============================================================================
void RotaryKnob::resized()
{
    slider.setTopLeftPosition(0, 24);
}

void RotaryKnob::setAlertLevel(float newAlert)
{
    alertLevel = newAlert;
    slider.repaint();
}

float RotaryKnob::getAlertLevel() const
{
    return alertLevel;
}

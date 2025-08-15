/*
  ==============================================================================

    RotaryKnob.cpp
    Created: 4 Jun 2025 8:30:46am
    Author:  kyleb

  ==============================================================================
*/

#include <JuceHeader.h>
#include "RotaryKnob.h"
#include "../LookAndFeel/RotaryKnobLAF.h"

//==============================================================================
/**
 * @brief Constructs a RotaryKnob component and attaches it to an AudioProcessorValueTreeState parameter.
 *
 * This constructor sets up the rotary slider style, text box, label, tooltip, and LookAndFeel,
 * and attaches the slider to the provided parameter ID in the given APVTS instance.
 *
 * @param text           The label text to display above the knob.
 * @param apvts          Reference to the AudioProcessorValueTreeState for parameter attachment.
 * @param parameterID    The parameter ID in the APVTS to attach this knob to.
 * @param drawFromMiddle If true, indicates that the knob should be drawn from its middle position.
 */
RotaryKnob::RotaryKnob(const juce::String& text,
    juce::AudioProcessorValueTreeState& apvts,
    const juce::ParameterID& parameterID,
    bool drawFromMiddle)
    : attachment(apvts, parameterID.getParamID(), slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, KnobDims::textBoxW, KnobDims::textBoxH);
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

    setName(text);
    setComponentID(parameterID.getParamID());
    slider.setTooltip(text);
    label.setMinimumHorizontalScale(1.0f);
    label.setInterceptsMouseClicks(false, false);

    setLookAndFeel(RotaryKnobLookAndFeel::get());
    setSize(KnobDims::width, KnobDims::height);
}

/**
 * @brief Destructor. Clears the LookAndFeel to prevent dangling pointers.
 */
RotaryKnob::~RotaryKnob()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
/**
 * @brief Called when the component is resized.
 *
 * Positions the slider within the knob's bounds, centered horizontally and offset from the top.
 */
void RotaryKnob::resized()
{
    auto area = getLocalBounds();

    slider.setBounds(
        (area.getWidth() - KnobDims::width) / 2,
        KnobDims::topOffset,
        KnobDims::width,
        area.getHeight() - KnobDims::topOffset);
}

/**
 * @brief Sets the alert level for the knob.
 *
 * The alert level is used by the LookAndFeel for visual feedback (e.g., color change).
 * Repaints the slider only if the new level differs from the current one by more than an epsilon.
 *
 * @param newAlert The new alert level value.
 */
void RotaryKnob::setAlertLevel(float newAlert)
{
    if (std::abs(newAlert - alertLevel) < 1e-6f)
        return;

    alertLevel = newAlert;
    slider.repaint();
}

/**
 * @brief Gets the current alert level value.
 *
 * @return The current alert level.
 */
float RotaryKnob::getAlertLevel() const
{
    return alertLevel;
}

/*
  ==============================================================================

    RotaryKnob.h
    Created: 4 Jun 2025 8:30:46am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <cmath>
#include <JuceHeader.h>

/**
 * @namespace KnobDims
 * @brief Contains constant dimensions for the RotaryKnob layout.
 *
 * These values define the fixed width, height, text box size,
 * and top offset for the knob within its component bounds.
 */
namespace KnobDims {
    constexpr int width = 57;  ///< Knob control width in pixels.
    constexpr int height = 100; ///< Total component height in pixels.
    constexpr int textBoxW = 57;  ///< Width of the slider's text box in pixels.
    constexpr int textBoxH = 13;  ///< Height of the slider's text box in pixels.
    constexpr int topOffset = 24;  ///< Vertical offset from the top to place the slider.
}

/**
 * @class RotaryKnob
 * @brief A rotary slider component with a label and optional alert level visualization.
 *
 * This component wraps a JUCE rotary Slider and Label, attaching the slider
 * to an AudioProcessorValueTreeState parameter for automation and state management.
 * It supports an "alert level" property used by the LookAndFeel for dynamic visual feedback.
 */
class RotaryKnob : public juce::Component
{
public:
    /**
     * @brief Constructs a RotaryKnob and attaches it to a parameter.
     *
     * @param text           The label text shown above the knob.
     * @param apvts          The AudioProcessorValueTreeState to attach the knob's slider to.
     * @param parameterID    The parameter ID in the APVTS to bind to this knob.
     * @param drawFromMiddle If true, indicates that the knob should be rendered from its middle position (bipolar).
     */
    RotaryKnob(const juce::String& text,
        juce::AudioProcessorValueTreeState& apvts,
        const juce::ParameterID& parameterID,
        bool drawFromMiddle = false);

    /**
     * @brief Destructor. Clears the LookAndFeel to prevent dangling references.
     */
    ~RotaryKnob() override;

    /**
     * @brief Called when the component is resized.
     *
     * Positions and sizes the internal slider according to the component's bounds
     * and the fixed dimensions defined in KnobDims.
     */
    void resized() override;

    /** @brief The underlying slider control. */
    juce::Slider slider;

    /** @brief The label displayed above the slider. */
    juce::Label label;

    /**
     * @brief Sets the current alert level for visual feedback.
     * @param newAlert The new alert level value.
     */
    void setAlertLevel(float newAlert);

    /**
     * @brief Returns the current alert level.
     * @return The stored alert level value.
     */
    float getAlertLevel() const;

    /**
     * @brief Returns the number of ticks to be drawn on the knob.
     * @return The number of ticks (default is 13).
     */
    virtual int getNumTicks() const noexcept { return 13; }

private:
    /** @brief Manages the connection between the slider and the APVTS parameter. */
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;

    /** @brief The current alert level value used for visual feedback. */
    float alertLevel = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryKnob)
};

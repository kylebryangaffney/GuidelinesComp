/*
  ==============================================================================

    RotaryKnobLAF.h
    Created: 15 Aug 2025 8:30:01am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../GUI/RotaryKnob.h"
#include "Fonts.h"
#include "Colors.h"
#include "LAFCommon.h"



//==============================================================================
// Knob Look and Feel

class RotaryKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RotaryKnobLookAndFeel();

    static RotaryKnobLookAndFeel* get()
    {
        static RotaryKnobLookAndFeel instance;
        return &instance;
    }


    // JUCE overrides
    void drawRotarySlider(juce::Graphics& g,
        int x, int y, int width, int height,
        float sliderPos,
        float rotaryStartAngle, float rotaryEndAngle,
        juce::Slider& slider) override;

    juce::Font getLabelFont(juce::Label& label) override;
    void fillTextEditorBackground(juce::Graphics& g,
        int width, int height,
        juce::TextEditor& textEditor) override;

    // Helpers
    void drawTicks(juce::Graphics& g,
        int numTicks,
        juce::Point<float> knobCenter,
        float tickRadius,
        float rotaryStartAngle,
        float rotaryEndAngle) noexcept;

    void drawKnobBody(juce::Graphics& g,
        const juce::Rectangle<float>& knobRect) noexcept;

    void drawArcTrack(juce::Graphics& g,
        const juce::Rectangle<float>& bounds,
        juce::Point<float> boundsCenter,
        float arcRadius,
        float rotaryStartAngle,
        float rotaryEndAngle,
        const juce::PathStrokeType& stroke) noexcept;

    void drawDialIndicator(juce::Graphics& g,
        juce::Point<float> knobCenter,
        float dialRadius,
        float toAngle,
        float lineWidth) noexcept;

    void drawValueArc(juce::Graphics& g,
        juce::Slider& slider,
        juce::Point<float> boundsCenter,
        const juce::PathStrokeType& stroke,
        float rotaryStartAngle,
        float rotaryEndAngle,
        float arcRadius,
        float toAngle,
        float alertLevel) noexcept;

    juce::Label* createSliderTextBox(juce::Slider& slider);


private:
    juce::DropShadow dropShadow{ Colors::Knob::dropShadow, 6, { 0, 3 } };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryKnobLookAndFeel)
};



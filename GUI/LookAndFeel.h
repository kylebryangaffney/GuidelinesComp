/*
  ==============================================================================

    LookAndFeel.h
    Created: 4 Jun 2025 8:10:41am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RotaryKnob.h"
#include "LevelMeter.h"

//==============================================================================
// Color Scheme
namespace Colors
{
    const juce::Colour background{ 30, 30, 30 };            // charcoal gray
    const juce::Colour header{ 50, 50, 50 };                // dark gray

    namespace Knob
    {
        const juce::Colour trackBackground{ 0, 0, 0 };             // black
        const juce::Colour outline{ 110, 40, 140 };                // regal purple
        const juce::Colour gradientTop{ 60, 60, 60 };              // mid gray
        const juce::Colour gradientBottom{ 30, 30, 30 };           // charcoal gray
        const juce::Colour dial{ 255, 255, 255 };                  // white
        const juce::Colour dropShadow{ 50, 50, 50 };               // dark gray
        const juce::Colour label{ 240, 240, 240 };                 // light gray
        const juce::Colour textBoxBackground{ 45, 45, 45 };        // deep gray
        const juce::Colour value{ 255, 255, 255 };                 // white
        const juce::Colour caret{ 255, 255, 255 };                 // white
        const juce::Colour tick{ 50, 200, 180 };                   // teal
    }

    namespace Group
    {
        const juce::Colour label{ 230, 120, 50 };                  // orange
        const juce::Colour outline{ 102, 51, 153 };                // plum
    }

    namespace LevelMeter
    {
        const juce::Colour background{ 20, 20, 20 };               // near-black
        const juce::Colour tickLine{ 200, 200, 200 };              // light gray
        const juce::Colour tickLabel{ 230, 120, 50 };              // orange
        const juce::Colour tooLoud{ 226, 74, 81 };                 // red
        const juce::Colour peakLevelOK{ 50, 200, 180 };            // aqua green
        const juce::Colour rmsLevelOK{ 30, 120, 60 };              // forest green
    }

    namespace Slider
    {
        const juce::Colour sliderFill{ 160, 70, 220 };             // violet
        const juce::Colour sliderClippingFill{ 226, 74, 81 };      // red
    }
}


//==============================================================================
// Font Style

class Fonts
{
public:
    Fonts() = delete;
    static juce::Font getFont(float height = 16.f);

private:
    static const juce::Typeface::Ptr typeface;
};

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

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider& slider) override;

    juce::Font getLabelFont(juce::Label&) override;
    juce::Label* createSliderTextBox(juce::Slider&) override;

    void drawTextEditorOutline(juce::Graphics&, int, int, juce::TextEditor&) override {}
    void fillTextEditorBackground(juce::Graphics&, int width, int height, juce::TextEditor&) override;

private:
    juce::DropShadow dropShadow{ Colors::Knob::dropShadow, 6, { 0, 3 } };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryKnobLookAndFeel)
};


//==============================================================================
// Level Meter Look and Feel

class LevelMeterLookAndFeel : public juce::LookAndFeel_V4
{
public:
    LevelMeterLookAndFeel() = default;

    static LevelMeterLookAndFeel* get()
    {
        static LevelMeterLookAndFeel instance;
        return &instance;
    }

    void drawLevelMeter(juce::Graphics& g, const LevelMeter& meter);
    void drawMeterBar(juce::Graphics& g, float levelDB, int x, int width,
        juce::Colour fillColour,
        std::function<int(float)> positionForLevel, int height);

    void drawPeakLevel(juce::Graphics& g, float level, int x, int width,
        std::function<int(float)> positionForLevel, int height);

    void drawRmsLevel(juce::Graphics& g, float level, int x, int width,
        std::function<int(float)> positionForLevel, int height);

private:
    const float clampdB = -120.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeterLookAndFeel)
};


//==============================================================================
// Main Look and Feel

class MainLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MainLookAndFeel();

    juce::Font getLabelFont(juce::Label&) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainLookAndFeel)
};

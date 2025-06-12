/*
  ==============================================================================

    LookAndFeel.h
    Created: 4 Jun 2025 8:10:41am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
// Color Scheme
namespace Colors
{
    const juce::Colour background{ 0, 0, 0 };           // Black background
    const juce::Colour header{ 50, 50, 50 };        // Dark gray for headers

    namespace Knob
    {
        const juce::Colour trackBackground{ 0, 0, 0 };           // Background behind knob track
        const juce::Colour trackActive{ 255, 120, 40 };      // Golden amber
        const juce::Colour outline{ 110, 40, 140 };      // Regal purple
        const juce::Colour gradientTop{ 255, 160, 110 };     // Soft coral
        const juce::Colour gradientBottom{ 255, 100, 60 };      // Hot tangerine
        const juce::Colour dial{ 150, 80, 200 };      // Luxurious violet
        const juce::Colour dropShadow{ 25, 25, 25 };        // Soft dark shadow
        const juce::Colour label{ 240, 240, 240 };     // Elegant light gray
        const juce::Colour textBoxBackground{ 45, 45, 45 };        // Slightly darker gray
        const juce::Colour value{ 255, 255, 255 };     // White
        const juce::Colour caret{ 255, 130, 50 };      // Sunset orange
    }

    namespace Group
    {
        const juce::Colour label{ 230, 120, 50 };  // Dark orange
        const juce::Colour outline{ 102, 51, 153 };  // Royal plum
    }

    namespace Button
    {
        const juce::Colour text{ 230, 120, 50 };  // Deeper orange
        const juce::Colour textToggled{ 255, 255, 255 }; // White when toggled
        const juce::Colour background{ 50, 50, 50 };    // Dark gray
        const juce::Colour backgroundToggled{ 102, 51, 153 };  // Royal plum when toggled
        const juce::Colour outline{ 230, 120, 50 };  // Orange outline
    }

    namespace LevelMeter
    {
        const juce::Colour background{ 20, 20, 20 };        // Very dark gray
        const juce::Colour tickLine{ 200, 200, 200 };      // Light gray ticks
        const juce::Colour tickLabel{ 230, 120, 50 };       // Orange labels
        const juce::Colour tooLoud{ 226, 74, 81 };        // Red for overload
        const juce::Colour peakLevelOK{ 255, 255, 0 };  // Bright yellow
        const juce::Colour rmsLevelOK{ 0, 122, 204 };   // Medium blue
    }

    namespace Slider
    {
        const juce::Colour sliderFill{ 160, 70, 220 };   // Purple accent
        const juce::Colour standoutFill{ 50, 200, 180 };   // Cyan highlight
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
// Main Look and Feel

class MainLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MainLookAndFeel();

    juce::Font getLabelFont(juce::Label&) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainLookAndFeel)
};

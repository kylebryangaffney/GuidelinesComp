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
#include "GainReductionMeter.h"
#include "PresetPanel.h"

//==============================================================================
// Color Scheme
namespace Colors
{
    const juce::Colour background{ 10, 10, 10 };            // charcoal gray
    const juce::Colour header{ 50, 50, 50 };                // dark gray

    namespace Knob
    {
        const juce::Colour trackBackground{ 40, 180, 160 };        // teal
        const juce::Colour outline{ 110, 40, 140 };                // regal purple
        const juce::Colour gradientTop{ 45, 45, 45 };              // mid gray
        const juce::Colour gradientBottom{ 30, 30, 30 };           // charcoal gray
        const juce::Colour dial{ 255, 255, 255 };                  // white
        const juce::Colour dropShadow{ 50, 50, 50 };               // dark gray
        const juce::Colour label{ 240, 240, 240 };                 // light gray
        const juce::Colour textBoxBackground{ 45, 45, 45 };        // deep gray
        const juce::Colour value{ 255, 255, 255 };                 // white
        const juce::Colour caret{ 255, 255, 255 };                 // white
        const juce::Colour tick{ 40, 180, 160 };                   // teal
    }

    namespace Group
    {
        const juce::Colour label{ 255, 140, 60 };                  // orange
        const juce::Colour outline{ 102, 51, 153 };                // plum
    }

    namespace PresetPanel
    {
        const juce::Colour outline{ 255, 255, 255};                // regal purple
        const juce::Colour background{ 45, 45, 45 };        // deep gray
        const juce::Colour boxBackground{ 80, 80, 80 };
        const juce::Colour buttonBase{ 160, 70, 220 };
        const juce::Colour buttonHover{ 255, 140, 60 };
        const juce::Colour buttonDown{ 226, 74, 81 };                 // red
        const juce::Colour text{255, 255, 255};
    }

    namespace LevelMeter
    {
        const juce::Colour background{ 0, 0, 0 };               // black
        const juce::Colour border{ 255, 255, 255 };
        const juce::Colour tickLine{ 225, 225, 225 };              // light gray
        const juce::Colour tickLabel{ 230, 230, 230 };              // orange
        const juce::Colour tooLoud{ 226, 74, 81 };                 // red
        const juce::Colour peakLevelOK{ 50, 200, 180 };            // aqua green
        const juce::Colour rmsLevelOK{ 30, 120, 60 };              // forest green
        const juce::Colour gainReduction{ 255, 140, 60 };          // orange

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
    static juce::Font getFont(float height = 14.f);

private:
    static const juce::Typeface::Ptr typeface;
};

class PresetPanelLookAndFeel : public juce::LookAndFeel_V4
{
public:
    PresetPanelLookAndFeel();

    void drawButtonBackground(juce::Graphics& g, juce::Button& b, const juce::Colour& backgroundColor,
        bool isMouseOverButton, bool isButtonDown) override;
    
    void drawButtonText(juce::Graphics& g,
        juce::TextButton& b,
        bool isMouseOverButton, bool isButtonDown) override;

    void drawComboBox(juce::Graphics& g,
        int width, int height, bool isButtonDown,
        int buttonX, int buttonY, int buttonW, int buttonH,
        juce::ComboBox& box) override;

    void drawPresetPanelBackground(juce::Graphics& g, juce::Rectangle<int> area);
    

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetPanelLookAndFeel)


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
    void drawTicks(juce::Graphics& g,
        int numTicks,
        juce::Point<float> knobCenter,
        float tickRadius,
        float rotaryStartAngle,
        float rotaryEndAngle);
    void drawKnobBody(juce::Graphics& g, juce::Rectangle<float> knobRect);
    void drawArcTrack(juce::Graphics& g, 
        juce::Rectangle<float> bounds, 
        juce::Point<float> boundsCenter,
        float arcRadius, 
        float rotaryStartAngle, 
        float rotaryEndAngle, 
        juce::PathStrokeType stroke);
    void drawDialIndicator(juce::Graphics& g,
        juce::Point<float> knobCenter,
        float dialRadius,
        float toAngle,
        float lineWidth);
    void drawValueArc(juce::Graphics& g,
        juce::Slider& slider,
        juce::Point<float> boundsCenter,
        juce::PathStrokeType stroke,
        float rotaryStartAngle, 
        float rotaryEndAngle, 
        float arcRadius, 
        float toAngle,
        float alertLevel
        );


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
    void drawMeterBar(int x, int y, int width, int height, juce::Graphics& g, float levelDB,
        std::function<int(float)> positionForLevel, juce::Colour fillColour);
    void drawPeakLevel(int x, int y, int width, int height,
        juce::Graphics& g,
        float levelDB,
        std::function<int(float)> positionForLevel);
    void drawRmsLevel(int x, int y, int width, int height,
        juce::Graphics& g,
        float levelDB,
        std::function<int(float)> positionForLevel);


private:
    const float clampdB = -120.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeterLookAndFeel)
};


//==============================================================================
// Gain Reduction Meter Look and Feel
class GainReductionMeterLookAndFeel : public juce::LookAndFeel_V4
{
public:
    GainReductionMeterLookAndFeel() = default;
    static GainReductionMeterLookAndFeel* get()
    {
        static GainReductionMeterLookAndFeel instance;
        return &instance;
    }

    void drawGainReductionMeter(juce::Graphics& g, const GainReductionMeter& meter);
    void drawMeterBar(int x, int y, int width, int height, juce::Graphics& g, float levelDB,
        std::function<int(float)> positionForLevel, juce::Colour fillColour);
    void drawRmsLevel(int x, int y, int width, int height,
        juce::Graphics& g,
        float levelDB,
        std::function<int(float)> positionForLevel);

private:
    const float clampdB = -120.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainReductionMeterLookAndFeel)
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

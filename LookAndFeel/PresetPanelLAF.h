/*
  ==============================================================================

    PresetPanelLAF.h
    Created: 15 Aug 2025 8:29:46am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include<JuceHeader.h>
#include "../GUI/PresetPanel.h"



//==================================
// PresetPanel

class PresetPanelLookAndFeel : public juce::LookAndFeel_V4
{
public:
    PresetPanelLookAndFeel();

    static PresetPanelLookAndFeel* get()
    {
        static PresetPanelLookAndFeel instance;
        return &instance;
    }

    void drawButtonBackground(juce::Graphics& g,
        juce::Button& b,
        const juce::Colour& backgroundColor,
        bool isMouseOverButton,
        bool isButtonDown) override;

    void drawButtonText(juce::Graphics& g,
        juce::TextButton& b,
        bool isMouseOverButton,
        bool isButtonDown) override;

    void drawComboBox(juce::Graphics& g,
        int width, int height,
        bool isButtonDown,
        int buttonX, int buttonY,
        int buttonW, int buttonH,
        juce::ComboBox& box) override;

    // Helper (not a JUCE override)
    void drawPresetPanelBackground(juce::Graphics& g,
        juce::Rectangle<int> area);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetPanelLookAndFeel)
};

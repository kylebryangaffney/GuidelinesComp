/*
  ==============================================================================

    Colors.h
    Created: 15 Aug 2025 8:28:34am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


//==============================================================================
// Color Scheme
namespace Colors
{
    const juce::Colour background{ 10, 10, 10 };
    const juce::Colour header{ 50, 50, 50 };

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
        const juce::Colour outline{ 70,  70,  70 };  // or (60, 60, 60)
        const juce::Colour background{ 44,  46,  50 };  // deep gray, almost black
        const juce::Colour boxBackground{ 56,  58,  62 };  // muted dark gray
        const juce::Colour saveButton{ 80, 130, 110 };   // muted grey-green
        const juce::Colour deleteButton{ 130,  60,  60 };   // muted grey-red
        const juce::Colour buttonBase{ 64,  82, 108 };  // slate blue-gray
        const juce::Colour buttonHover{ 96, 120, 156 };  // muted blue, gentle highlight
        const juce::Colour buttonDown{ 46,  56,  76 };  // darker blue, low saturation
        const juce::Colour text{ 230, 232, 235 }; // soft light gray
        const juce::Colour saveGreen{ 64, 180, 144 };  // calm green, readable on dark
        const juce::Colour deleteRed{ 170, 60, 60 };

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

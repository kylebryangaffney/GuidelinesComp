/*
  ==============================================================================

    CheckmarkButton.h
    Created: 24 Jul 2025 9:14:31am
    Author:  kyleb

  ==============================================================================
*/


#pragma once

#include <JuceHeader.h>

class CheckmarkButton : public juce::TextButton
{
public:
    CheckmarkButton();
    void paintButton(juce::Graphics& g, bool isHighlighted, bool isDown) override;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CheckmarkButton)
};


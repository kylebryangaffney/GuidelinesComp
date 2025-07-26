/*
  ==============================================================================

    XButton.h
    Created: 24 Jul 2025 9:16:15am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class XButton : public juce::Button
{
public:
    XButton();
    void paintButton(juce::Graphics& g, bool isHighlighted, bool isDown) override;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XButton)

};

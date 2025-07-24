/*
  ==============================================================================

    CheckmarkButton.cpp
    Created: 24 Jul 2025 9:14:31am
    Author:  kyleb

  ==============================================================================
*/

#include "CheckmarkButton.h"

#include "LookAndFeel.h"

CheckmarkButton::CheckmarkButton()
{
    setButtonText(u8"\u2714"); // Unicode Checkmark
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void CheckmarkButton::paintButton(juce::Graphics& g, bool isHighlighted, bool isDown)
{
    auto bounds = getLocalBounds().toFloat();
    float corner = 4.0f;

    g.setColour(Colors::PresetPanel::buttonBase);
    g.fillRoundedRectangle(bounds, corner);

    if (isDown)
        g.setColour(Colors::PresetPanel::buttonDown.withAlpha(0.4f));
    else if (isHighlighted)
        g.setColour(Colors::PresetPanel::buttonHover.withAlpha(0.3f));
    else
        g.setColour(Colors::PresetPanel::outline.withAlpha(0.f)); // transparent if no hover

    g.fillRoundedRectangle(bounds, corner);

    g.setColour(Colors::PresetPanel::outline);
    g.drawRoundedRectangle(bounds, corner, 1.5f);

    // Set unique color for checkmark
    g.setColour(Colors::PresetPanel::saveGreen);
    g.setFont(12.0f);
    g.drawFittedText(getButtonText(), getLocalBounds(), juce::Justification::centred, 1);
}
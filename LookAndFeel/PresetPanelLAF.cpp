/*
  ==============================================================================

    PresetPanelLAF.cpp
    Created: 15 Aug 2025 8:29:46am
    Author:  kyleb

  ==============================================================================
*/

#include "PresetPanelLAF.h"
#include "Colors.h"



//================================================================================
// PresetPanelLookAndFeel

/**
 * @brief Constructs the PresetPanel look-and-feel and sets default control colours.
 * Applies colours for ComboBox and TextButton used within the preset panel UI.
 */
PresetPanelLookAndFeel::PresetPanelLookAndFeel()
{
    setColour(juce::ComboBox::backgroundColourId, Colors::PresetPanel::boxBackground);
    setColour(juce::ComboBox::outlineColourId, Colors::PresetPanel::outline);
    setColour(juce::ComboBox::textColourId, Colors::PresetPanel::text);

    setColour(juce::TextButton::buttonColourId, Colors::PresetPanel::buttonBase);
    setColour(juce::TextButton::textColourOnId, Colors::PresetPanel::text);
    setColour(juce::TextButton::textColourOffId, Colors::PresetPanel::text);
}

/**
 * @brief Paints a TextButton background with rounded corners and hover/press overlays.
 * @param g               Graphics context.
 * @param b               The button to paint.
 * @param backgroundColor Ignored; the LookAndFeel colours are used instead.
 * @param isMouseOverButton True if the mouse is hovering the button.
 * @param isButtonDown      True if the button is currently pressed.
 * @note Uses semi-transparent overlays for hover/press states and draws an outline.
 */
void PresetPanelLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& b,
    [[maybe_unused]] const juce::Colour& backgroundColor,
    bool isMouseOverButton, bool isButtonDown)
{
    auto  bounds = b.getLocalBounds().toFloat();
    float corner = 4.0f;

    g.setColour(b.findColour(juce::TextButton::buttonColourId));
    g.fillRoundedRectangle(bounds, corner);

    if (isButtonDown)
    {
        g.setColour(Colors::PresetPanel::buttonDown.withAlpha(0.4f));
        g.fillRoundedRectangle(bounds, corner);
    }
    else if (isMouseOverButton)
    {
        g.setColour(Colors::PresetPanel::buttonHover.withAlpha(0.3f));
        g.fillRoundedRectangle(bounds, corner);
    }

    g.setColour(Colors::PresetPanel::outline);
    g.drawRoundedRectangle(bounds, corner, 1.5f);
}

/**
 * @brief Draws the text for a juce::TextButton using on/off colours based on toggle state.
 * @param g  Graphics context.
 * @param b  The TextButton to draw text for.
 * @param isMouseOverButton Unused.
 * @param isButtonDown      Unused.
 */
void PresetPanelLookAndFeel::drawButtonText(juce::Graphics& g,
    juce::TextButton& b,
    [[maybe_unused]] bool isMouseOverButton, [[maybe_unused]] bool isButtonDown)
{
    juce::Colour buttonColor = b.findColour(b.getToggleState()
        ? juce::TextButton::textColourOnId
        : juce::TextButton::textColourOffId);

    g.setColour(buttonColor);
    g.drawFittedText(b.getButtonText(), b.getLocalBounds(), juce::Justification::centred, 1);
}

/**
 * @brief Paints the ComboBox background and outline for the preset list.
 * @param g         Graphics context.
 * @param width     Width of the ComboBox area.
 * @param height    Height of the ComboBox area.
 * @param isButtonDown Unused.
 * @param buttonX   Unused (button glyph X).
 * @param buttonY   Unused (button glyph Y).
 * @param buttonW   Unused (button glyph width).
 * @param buttonH   Unused (button glyph height).
 * @param box       The ComboBox to paint.
 * @note This override does not draw a drop-down arrow glyph.
 */
void PresetPanelLookAndFeel::drawComboBox(juce::Graphics& g,
    int width, int height, [[maybe_unused]] bool isButtonDown,
    [[maybe_unused]] int buttonX, [[maybe_unused]] int buttonY,
    [[maybe_unused]] int buttonW, [[maybe_unused]] int buttonH,
    juce::ComboBox& box)
{
    auto cornerSize = 4.0f;
    juce::Rectangle<int> boxBounds(0, 0, width, height);

    g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

    g.setColour(Colors::PresetPanel::outline);
    g.drawRoundedRectangle(boxBounds.toFloat(), cornerSize, 1.5f);
}

/**
 * @brief Fills the Preset Panel's rounded background.
 * @param g    Graphics context.
 * @param area Rectangle area to fill (in component coordinates).
 * @note The containing component is expected to handle outer padding/margins.
 */
void PresetPanelLookAndFeel::drawPresetPanelBackground(juce::Graphics& g,
    juce::Rectangle<int> area)
{
    g.setColour(Colors::PresetPanel::background);
    g.fillRoundedRectangle(area.toFloat(), 7.0f);
}


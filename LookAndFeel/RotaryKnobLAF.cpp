/*
  ==============================================================================

    RotaryKnobLAF.cpp
    Created: 15 Aug 2025 8:30:01am
    Author:  kyleb

  ==============================================================================
*/

#include "RotaryKnobLAF.h"



/**
 * @class RotaryKnobLabel
 * @brief A custom Label class for rotary knob controls with restricted editing behavior.
 *
 * This label is designed for use with rotary knobs, disabling mouse wheel edits,
 * streamlining accessibility, and customizing the in-place editor to match the
 * knob's style and expected input restrictions.
 */
class RotaryKnobLabel final : public juce::Label
{
public:
    /**
     * @brief Default constructor.
     *
     * Initializes the label with default behavior.
     */
    RotaryKnobLabel() = default;

    /**
     * @brief Overrides mouse wheel movement behavior.
     *
     * Intentionally ignores mouse wheel movement to prevent accidental value changes
     * when scrolling over the label.
     *
     * @param event Unused mouse event details.
     * @param wheel Unused mouse wheel details.
     */
    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails&) override
    {
        // No operation - prevent accidental scrolling changes.
    }

    /**
     * @brief Creates an accessibility handler for assistive technologies.
     *
     * Returns an ignored accessibility handler so the label does not interfere
     * with screen readers or other assistive input systems when editing.
     *
     * @return A unique_ptr to the accessibility handler.
     */
    std::unique_ptr<juce::AccessibilityHandler> createAccessibilityHandler() override
    {
        return createIgnoredAccessibilityHandler(*this);
    }

    /**
     * @brief Creates the custom in-place text editor for label editing.
     * Configures the text editor to:
     * - Use the label's font and colors.
     * - Remove unnecessary borders and apply compact padding.
     * - Disable the popup menu.
     * - Use numeric-friendly input restrictions.
     * - Center the text vertically and horizontally (or top-aligned if desired).
     * - Show a numeric keypad on touch devices.
     * JUCE takes ownership of the returned editor pointer.
     * @return A raw pointer to the newly created TextEditor instance.
     */
    juce::TextEditor* createEditorComponent() override
    {
        auto* ed = new juce::TextEditor(getName());

        // Match label font & colours
        ed->applyFontToAllText(getLookAndFeel().getLabelFont(*this));
        copyAllExplicitColoursTo(*ed);

        // Compact, centered editor
        ed->setBorder(juce::BorderSize<int>());               // No extra border
        ed->setIndents(2, 1);                                 // Slight padding
        ed->setJustification(juce::Justification::centred);   // Or centredTop if preferred

        // UX niceties
        ed->setPopupMenuEnabled(false);
        ed->setReturnKeyStartsNewLine(false);
        ed->setScrollbarsShown(false);
        ed->setSelectAllWhenFocused(true);
        ed->setCaretVisible(true);

        // Numeric-friendly input restrictions
        // First arg = max chars; second arg = allowed chars (optional)
        ed->setInputRestrictions(6, "0123456789.-");

        // On touch devices / soft keyboards, prefer numeric keypad
        ed->setKeyboardType(juce::TextInputTarget::decimalKeyboard);

        return ed;
    }
};


//==============================================================================
// RotaryKnobLookAndFeel

/**
 * @brief Constructs the RotaryKnob look-and-feel.
 * Sets default colours for labels, sliders, text boxes, and caret
 * components used in rotary knob controls.
 */
RotaryKnobLookAndFeel::RotaryKnobLookAndFeel()
{
    setColour(juce::Label::textColourId, Colors::Knob::label);
    setColour(juce::Slider::textBoxTextColourId, Colors::Knob::label);
    setColour(juce::Slider::rotarySliderFillColourId, Colors::Slider::sliderFill);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::CaretComponent::caretColourId, Colors::Knob::caret);
}

//==============================================================================
// Knob rendering logic

/**
 * @brief Draws a complete rotary slider with ticks, body, gradient fill,
 *        arc track, dial indicator, and value arc.
 * @param g                 Graphics context.
 * @param x                 X position of the knob bounds.
 * @param y                 Y position of the knob bounds.
 * @param width             Width of the knob bounds.
 * @param height            Height of the knob bounds (unused).
 * @param sliderPos         Normalised slider position in range [0, 1].
 * @param rotaryStartAngle  Start angle of the rotary range in radians.
 * @param rotaryEndAngle    End angle of the rotary range in radians.
 * @param slider            Slider reference to draw.
 * @note This method will look for a parent RotaryKnob component to query
 *       tick count and alert level. The value arc is drawn only if the slider
 *       is enabled.
 */
void RotaryKnobLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width,
    [[maybe_unused]] int height, float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle,
    juce::Slider& slider)
{
    const juce::Rectangle<float> bounds{ (float)x, (float)y, (float)width, (float)width };
    const juce::Rectangle<float> knobRect = bounds.reduced(10.f);
    const juce::Point<float>     knobCenter = knobRect.getCentre();

    auto* knob = slider.findParentComponentOfClass<RotaryKnob>();
    const float  alertLevel = knob ? knob->getAlertLevel() : 0.0f;
    const int    numTicks = knob ? knob->getNumTicks() : 0;
    const float  tickRadius = knobRect.getWidth() * 0.5f + 2.0f;

    drawTicks(g, numTicks, knobCenter, tickRadius, rotaryStartAngle, rotaryEndAngle);
    drawKnobBody(g, knobRect);

    const auto innerRect = knobRect.reduced(2.f);
    juce::ColourGradient gradient(
        Colors::Knob::gradientTop, innerRect.getCentreX(), innerRect.getY(),
        Colors::Knob::gradientBottom, innerRect.getCentreX(), innerRect.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillEllipse(innerRect);

    const auto bevelRect = innerRect.reduced(1.5f);
    g.setColour(juce::Colours::white.withAlpha(0.08f));
    g.drawEllipse(bevelRect, 1.0f);

    const juce::Point<float> boundsCenter = bounds.getCentre();
    const float radius = bounds.getWidth() * 0.5f;
    constexpr float lineWidth = 3.0f;
    const float arcRadius = radius - lineWidth * 0.5f;

    const juce::PathStrokeType stroke(lineWidth,
        juce::PathStrokeType::curved,
        juce::PathStrokeType::rounded);
    drawArcTrack(g, bounds, boundsCenter, arcRadius, rotaryStartAngle, rotaryEndAngle, stroke);

    const float span = (rotaryEndAngle - rotaryStartAngle);
    const float pos01 = juce::jlimit(0.0f, 1.0f, sliderPos);
    const float toAngle = rotaryStartAngle + pos01 * span;
    const float dialRadius = innerRect.getHeight() * 0.5f - lineWidth;

    drawDialIndicator(g, knobCenter, dialRadius, toAngle, lineWidth);

    if (slider.isEnabled())
        drawValueArc(g, slider, boundsCenter, stroke, rotaryStartAngle, rotaryEndAngle, arcRadius, toAngle, alertLevel);
}

/**
 * @brief Draws evenly spaced tick marks around the knob.
 * @param g                 Graphics context.
 * @param numTicks          Number of ticks to draw (minimum 2).
 * @param knobCenter        Center point of the knob.
 * @param tickRadius        Distance from knob center to start of each tick.
 * @param rotaryStartAngle  Start angle of the tick arc in radians.
 * @param rotaryEndAngle    End angle of the tick arc in radians.
 */
void RotaryKnobLookAndFeel::drawTicks(juce::Graphics& g,
    int numTicks,
    juce::Point<float> knobCenter,
    float tickRadius,
    float rotaryStartAngle,
    float rotaryEndAngle) noexcept
{
    constexpr float tickLength = 6.0f;
    constexpr float tickThickness = 1.0f;

    jassert(numTicks >= 2);
    jassert(tickRadius > 0.0f);
    jassert(rotaryEndAngle != rotaryStartAngle);

    if (numTicks < 2 || tickRadius <= 0.0f || rotaryEndAngle == rotaryStartAngle)
        return;

    const float invDen = 1.0f / float(numTicks - 1);
    const float span = (rotaryEndAngle - rotaryStartAngle);

    g.setColour(Colors::Knob::tick);

    for (int i = 0; i < numTicks; ++i)
    {
        const float t = i * invDen;
        const float angle = rotaryStartAngle + t * span;

        const auto p1 = knobCenter.getPointOnCircumference(tickRadius, angle);
        const auto p2 = knobCenter.getPointOnCircumference(tickRadius + tickLength, angle);

        g.drawLine({ p1, p2 }, tickThickness);
    }
}

/**
 * @brief Draws the main knob body shape and its drop shadow.
 * @param g         Graphics context.
 * @param knobRect  Bounds of the knob body.
 */
void RotaryKnobLookAndFeel::drawKnobBody(juce::Graphics& g, const juce::Rectangle<float>& knobRect) noexcept
{
    juce::Path knobShape;
    knobShape.addEllipse(knobRect);
    dropShadow.drawForPath(g, knobShape);

    g.setColour(Colors::Knob::outline);
    g.fillPath(knobShape);
}

/**
 * @brief Draws the background arc track behind the knob value arc.
 * @param g                 Graphics context.
 * @param bounds            Bounds of the knob (unused).
 * @param boundsCenter      Center point of the knob.
 * @param arcRadius         Radius of the arc.
 * @param rotaryStartAngle  Start angle in radians.
 * @param rotaryEndAngle    End angle in radians.
 * @param stroke            Stroke style for the arc.
 */
void RotaryKnobLookAndFeel::drawArcTrack(juce::Graphics& g,
    [[maybe_unused]] const juce::Rectangle<float>& bounds,
    juce::Point<float> boundsCenter,
    float arcRadius,
    float rotaryStartAngle,
    float rotaryEndAngle,
    const juce::PathStrokeType& stroke) noexcept
{
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(boundsCenter.x, boundsCenter.y, arcRadius, arcRadius, 0.f,
        rotaryStartAngle, rotaryEndAngle, true);

    g.setColour(Colors::Knob::trackBackground);
    g.strokePath(backgroundArc, stroke);
}

/**
 * @brief Draws the knob's dial indicator (caret) and optional inner ring.
 * @param g           Graphics context.
 * @param knobCenter  Center point of the knob.
 * @param dialRadius  Radius from center to the indicator tip.
 * @param toAngle     Angle of the indicator in radians.
 * @param lineWidth   Stroke width for the indicator and ring.
 */
void RotaryKnobLookAndFeel::drawDialIndicator(juce::Graphics& g,
    juce::Point<float> knobCenter,
    float dialRadius,
    float toAngle,
    float lineWidth) noexcept
{
    const float s = std::sin(toAngle);
    const float c = std::cos(toAngle);

    const juce::Point<float> dialStart(knobCenter.x + 2.f * s,
        knobCenter.y - 2.f * c);
    const juce::Point<float> dialEnd(knobCenter.x + dialRadius * s,
        knobCenter.y - dialRadius * c);

    juce::Path dialPath;
    dialPath.startNewSubPath(dialStart);
    dialPath.lineTo(dialEnd);

    g.setColour(Colors::Knob::caret);
    g.strokePath(dialPath, juce::PathStrokeType(3.0f));

    const juce::Rectangle<float> innerCircleRect(
        knobCenter.x - dialRadius,
        knobCenter.y - dialRadius,
        dialRadius * 2,
        dialRadius * 2);

    g.drawEllipse(innerCircleRect, lineWidth);
}

/**
 * @brief Draws the active value arc indicating the knob's current position.
 * @param g                 Graphics context.
 * @param slider            Slider reference (checked for "drawFromMiddle" property).
 * @param boundsCenter      Center point of the knob.
 * @param stroke            Stroke style for the value arc.
 * @param rotaryStartAngle  Start angle in radians.
 * @param rotaryEndAngle    End angle in radians.
 * @param arcRadius         Radius of the arc.
 * @param toAngle           End angle of the active arc in radians.
 * @param alertLevel        Value in [0, 1] blending normal and clipping fill colours.
 */
void RotaryKnobLookAndFeel::drawValueArc(juce::Graphics& g,
    juce::Slider& slider,
    juce::Point<float> boundsCenter,
    const juce::PathStrokeType& stroke,
    float rotaryStartAngle,
    float rotaryEndAngle,
    float arcRadius,
    float toAngle,
    float alertLevel) noexcept
{
    float fromAngle = rotaryStartAngle;
    if (slider.getProperties()["drawFromMiddle"])
        fromAngle += (rotaryEndAngle - rotaryStartAngle) / 2.f;

    juce::Path valueArc;
    valueArc.addCentredArc(boundsCenter.x, boundsCenter.y, arcRadius, arcRadius, 0.f,
        fromAngle, toAngle, true);

    juce::Colour finalFillColor = Colors::Slider::sliderFill
        .interpolatedWith(Colors::Slider::sliderClippingFill, alertLevel);

    g.setColour(finalFillColor);
    g.strokePath(valueArc, stroke);
}

//==============================================================================
// Label and text box styling for rotary sliders

/**
 * @brief Retrieves the font used for knob labels.
 * @param label Label reference (unused).
 * @return The font to use for labels.
 */
juce::Font RotaryKnobLookAndFeel::getLabelFont([[maybe_unused]] juce::Label& label)
{
    return Fonts::getFont();
}

/**
 * @brief Fills the background of the knob's text box editor.
 * @param g           Graphics context.
 * @param width       Width of the editor bounds (unused).
 * @param height      Height of the editor bounds (unused).
 * @param textEditor  Reference to the text editor to fill.
 */
void RotaryKnobLookAndFeel::fillTextEditorBackground(juce::Graphics& g,
    [[maybe_unused]] int width,
    [[maybe_unused]] int height,
    juce::TextEditor& textEditor)
{
    g.setColour(Colors::Knob::textBoxBackground);
    g.fillRoundedRectangle(textEditor.getLocalBounds().reduced(4, 0).toFloat(), 4.f);
}

// Custom label for rotary slider value entry
juce::Label* RotaryKnobLookAndFeel::createSliderTextBox(juce::Slider& slider)
{
    auto* label = new RotaryKnobLabel();

    // Readability & input UX
    label->setJustificationType(juce::Justification::centred);
    label->setKeyboardType(juce::TextInputTarget::decimalKeyboard);

    // Colours for label (view mode) and its editor (edit mode).
    // The RotaryKnobLabel::createEditorComponent() calls copyAllExplicitColoursTo(*ed),
    // so these TextEditor colour IDs will also apply to the in-place editor.
    label->setColour(juce::Label::textColourId,
        slider.findColour(juce::Slider::textBoxTextColourId));

    label->setColour(juce::TextEditor::textColourId, Colors::Knob::value);
    label->setColour(juce::TextEditor::highlightedTextColourId, Colors::Knob::value);
    label->setColour(juce::TextEditor::backgroundColourId, Colors::Knob::textBoxBackground);
    label->setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);

    // Use the slider’s fill colour as the selection highlight background.
    label->setColour(juce::TextEditor::highlightColourId,
        slider.findColour(juce::Slider::rotarySliderFillColourId));

    return label;
}

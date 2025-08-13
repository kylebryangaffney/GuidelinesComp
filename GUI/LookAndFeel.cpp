/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 4 Jun 2025 8:10:41am
    Author:  kyleb

  ==============================================================================
*/

#include "LookAndFeel.h"


//==============================================================================
// Fonts

/** @brief Font height used for meter tick labels. */
constexpr float tickFontHeight = 8.0f;

/** @brief Process-wide cached LatoMedium typeface loaded from binary data. */
const juce::Typeface::Ptr Fonts::typeface = juce::Typeface::createSystemTypefaceFor(
    BinaryData::LatoMedium_ttf, BinaryData::LatoMedium_ttfSize);

/**
 * @brief Returns a styled font using the cached typeface.
 * @param height Desired font height in points/pixels (JUCE units).
 * @return A juce::Font configured with the cached typeface and height.
 */
juce::Font Fonts::getFont(float height)
{
    return juce::FontOptions(typeface)
        .withMetricsKind(juce::TypefaceMetricsKind::legacy)
        .withHeight(height);
}


//==============================================================================
// Helpers

/**
 * @brief Draws a vertical tick and centers a numeric label at the given x position.
 * @param g           Graphics context to draw into.
 * @param x           X coordinate where the tick is drawn.
 * @param tickTop     Y coordinate of the top of the tick line.
 * @param tickHeight  Height of the tick line in pixels.
 * @param label       Text to draw under the tick.
 * @param labelTop    Y coordinate of the top of the label area.
 * @param labelWidth  Width of the label bounding box (used for centering).
 * @param labelHeight Height of the label bounding box.
 */
namespace
{
    void drawTickWithLabel(juce::Graphics& g, int x, int tickTop, int tickHeight,
        const juce::String& label, int labelTop, int labelWidth, int labelHeight)
    {
        g.setColour(Colors::LevelMeter::tickLine);
        g.fillRect(x, tickTop, 1, tickHeight);

        g.setColour(Colors::LevelMeter::tickLabel);
        g.drawText(label,
            x - labelWidth / 2, labelTop, labelWidth, labelHeight,
            juce::Justification::centred);
    }
}


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

//==============================================================================
// LevelMeterLookAndFeel

/**
 * @brief Draws the full level meter, including peak and RMS bars, borders, background, and tick labels.
 * @param g       The JUCE graphics context to use for rendering.
 * @param meter   Reference to the LevelMeter component providing measurement data and scale mapping.
 *
 * @note This method draws both channels' peak and RMS meters, applies background/border styling,
 *       and places dB tick marks along the scale.
 */
void LevelMeterLookAndFeel::drawLevelMeter(juce::Graphics& g, const LevelMeter& meter) noexcept
{
    const auto font = Fonts::getFont(tickFontHeight);
    constexpr float borderThickness = 1.0f;
    constexpr float borderRadius = 4.0f;

    const auto borderRect = meter.getLocalBounds().toFloat().reduced(7.0f);
    const auto innerRect = borderRect.reduced(10.0f);

    // Draw background and border
    g.fillAll(Colors::LevelMeter::background);
    g.setColour(Colors::LevelMeter::border);
    g.drawRoundedRectangle(borderRect, borderRadius, borderThickness);
    g.setFont(font);

    // Bar layout configuration
    constexpr int barHeight = 7;
    constexpr int rmsHeight = 4;
    constexpr int barSpacing = 2; // space between bars

    // Calculate bar X geometry
    const int xLeft = juce::roundToInt(innerRect.getX());
    const int xRight = juce::roundToInt(innerRect.getRight());
    const int w = xRight - xLeft;

    // Skip drawing if width is invalid
    if (w <= 0)
        return;

    // Calculate bar Y positions
    const int yPeakL = juce::roundToInt(innerRect.getY());
    const int yPeakR = yPeakL + barHeight + barSpacing;
    const int yRmsL = yPeakL;
    const int yRmsR = yPeakR;

    // Lambda to map dB values to pixel X positions
    const auto map = [xLeft, xRight, &meter](float db) noexcept
        {
            return meter.positionForLevel(db, xLeft, xRight);
        };

    // Draw peak levels
    drawPeakLevel(xLeft, yPeakL, w, barHeight, g, meter.getPeakLevelL(), map);
    drawPeakLevel(xLeft, yPeakR, w, barHeight, g, meter.getPeakLevelR(), map);

    // Draw RMS levels
    drawRmsLevel(xLeft, yRmsL, w, rmsHeight, g, meter.getRmsLevelL(), map);
    drawRmsLevel(xLeft, yRmsR, w, rmsHeight, g, meter.getRmsLevelR(), map);

    // Draw tick marks and numeric labels
    const int tickTop = yPeakL + 2; // offset down from top
    constexpr int tickHeight = 14;
    const int labelTop = tickTop + tickHeight;
    constexpr int labelHeight = 12;
    constexpr int labelWidth = 25;

    for (float db = meter.maxdB; db >= meter.mindB; db -= meter.stepdB)
    {
        const int x = map(db);
        drawTickWithLabel(g, x, tickTop, tickHeight,
            juce::String(int(db)),
            labelTop, labelWidth, labelHeight);
    }
}

/**
 * @brief Draws a single horizontal meter bar segment for either RMS or peak readings.
 * @param x                  Left X coordinate of the bar.
 * @param y                  Top Y coordinate of the bar.
 * @param width              Width of the bar in pixels.
 * @param height             Height of the bar in pixels.
 * @param g                  JUCE graphics context for drawing.
 * @param levelDB            The signal level in decibels (dB).
 * @param positionForLevel   Function mapping dB values to X coordinates.
 * @param fillColour         Colour to fill for levels below 0 dBFS.
 *
 * @note Levels above 0 dBFS are drawn in Colors::LevelMeter::tooLoud.
 */
void LevelMeterLookAndFeel::drawMeterBar(int x, int y, int width, int height,
    juce::Graphics& g, float levelDB,
    const std::function<int(float)>& positionForLevel,
    juce::Colour fillColour) noexcept
{
    if (!std::isfinite(levelDB) || levelDB <= clampdB || width <= 0 || height <= 0)
        return;

    jassert(x + width >= x); // geometry sanity check

    // Clamp pixel position to the bar's region
    const auto clampToBar = [x, width, &positionForLevel](float db) noexcept -> int
        {
            return juce::jlimit(x, x + width, positionForLevel(db));
        };

    const int xStart = clampToBar(levelDB);
    const int xZero = clampToBar(0.0f);

    if (xStart > xZero)
    {
        // Below 0 dBFS region
        g.setColour(fillColour);
        g.fillRect(x, y, xZero - x, height);

        // Above 0 dBFS region
        g.setColour(Colors::LevelMeter::tooLoud);
        g.fillRect(xZero, y, xStart - xZero, height);
    }
    else
    {
        // Entire bar is below 0 dBFS
        g.setColour(fillColour);
        g.fillRect(x, y, xStart - x, height);
    }
}

/**
 * @brief Draws a peak level bar for one channel.
 * @param x                  Left X coordinate.
 * @param y                  Top Y coordinate.
 * @param width              Width of the peak meter bar.
 * @param height             Height of the peak meter bar.
 * @param g                  JUCE graphics context.
 * @param levelDB            Peak level in dB.
 * @param positionForLevel   Mapping function from dB to X pixel position.
 */
void LevelMeterLookAndFeel::drawPeakLevel(int x, int y, int width, int height,
    juce::Graphics& g,
    float levelDB,
    const std::function<int(float)>& positionForLevel) noexcept
{
    drawMeterBar(x, y, width, height, g, levelDB, positionForLevel,
        Colors::LevelMeter::peakLevelOK);
}

/**
 * @brief Draws an RMS level bar for one channel.
 * @param x                  Left X coordinate.
 * @param y                  Top Y coordinate.
 * @param width              Width of the RMS meter bar.
 * @param height             Height of the RMS meter bar.
 * @param g                  JUCE graphics context.
 * @param levelDB            RMS level in dB.
 * @param positionForLevel   Mapping function from dB to X pixel position.
 */
void LevelMeterLookAndFeel::drawRmsLevel(int x, int y, int width, int height,
    juce::Graphics& g,
    float levelDB,
    const std::function<int(float)>& positionForLevel) noexcept
{
    drawMeterBar(x, y, width, height, g, levelDB, positionForLevel,
        Colors::LevelMeter::rmsLevelOK);
}
//==============================================================================
// GainReductionMeterLookAndFeel

/**
 * @brief Paints the full Gain Reduction meter: frame, bar and tick labels.
 *
 * Renders the meter background/border, a single horizontal GR bar, and evenly
 * spaced dB tick marks with numeric labels. Geometry is snapped to integers for
 * crisp 1-px lines. Uses the meter's mapping function to convert dB values to
 * pixel X positions.
 *
 * @param g      Graphics context to draw into.
 * @param meter  Source of layout and dB->pixel mapping (positionForLevel).
 */
void GainReductionMeterLookAndFeel::drawGainReductionMeter(juce::Graphics& g,
    const GainReductionMeter& meter) noexcept
{
    const auto  font = Fonts::getFont(tickFontHeight);
    constexpr float borderThickness = 1.0f;
    constexpr float borderRadius = 4.0f;

    const auto borderRect = meter.getLocalBounds().toFloat().reduced(7.0f);
    const auto innerRect = borderRect.reduced(10.0f);

    g.fillAll(Colors::LevelMeter::background);
    g.setColour(Colors::LevelMeter::border);
    g.drawRoundedRectangle(borderRect, borderRadius, borderThickness);
    g.setFont(font);

    // Bar settings
    constexpr int rmsHeight = 4;

    // Snap geometry to integers for crisp 1-px lines
    const int xLeft = juce::roundToInt(innerRect.getX());
    const int xRight = juce::roundToInt(innerRect.getRight());
    const int yTop = juce::roundToInt(innerRect.getY());
    const int w = xRight - xLeft;
    if (w <= 0) return;

    /// Mapper: dB -> pixel x
    const auto map = [xLeft, xRight, &meter](float db) noexcept -> int
        {
            return meter.positionForLevel(db, xLeft, xRight);
        };

    // Draw the GR bar (thin RMS-style bar that reflects current max GR level)
    drawRmsLevel(xLeft, yTop, w, rmsHeight, g, meter.getMaxRmsLevel(), map);

    // Ticks and labels
    const int  tickTop = yTop + 2;  // 2 px down from top of inner rect
    constexpr int tickHeight = 8;
    const int  labelTop = tickTop + tickHeight;
    constexpr int labelHeight = 12;
    constexpr int labelWidth = 27;

    for (float db = GainReductionMeter::maxdB;
        db >= GainReductionMeter::mindB - 1.0e-3f;
        db -= GainReductionMeter::stepdB)
    {
        const int x = map(db);
        drawTickWithLabel(g, x, tickTop, tickHeight,
            juce::String(int(db)),
            labelTop, labelWidth, labelHeight);
    }
}

/**
 * @brief Draws a horizontal gain-reduction bar segment between two mapped X positions.
 *
 * Computes the pixel positions for "no reduction" (typically far right) and the
 * current clamped GR level, clamps them to the bar bounds, and fills the span.
 *
 * @param x               Left bar bound in pixels.
 * @param y               Top bar bound in pixels.
 * @param width           Bar width in pixels.
 * @param height          Bar height in pixels.
 * @param g               Graphics context to draw into.
 * @param levelDB         Current gain-reduction level in dB (negative domain).
 * @param positionForLevel Callable mapping dB -> pixel X within [x, x + width].
 * @param fillColour      Colour used to fill the bar.
 */
void GainReductionMeterLookAndFeel::drawMeterBar(
    int x, int y, int width, int height, juce::Graphics& g, float levelDB,
    const std::function<int(float)>& positionForLevel, juce::Colour fillColour) noexcept
{
    if (width <= 0 || height <= 0)
        return;

    const float clampedDB = juce::jlimit(GainReductionMeter::mindB,
        GainReductionMeter::maxdB,
        levelDB);

    // X of “no reduction” (typically the far right) and X of current GR
    const int xNoReduction = juce::jlimit(x, x + width,
        positionForLevel(GainReductionMeter::maxdB));
    const int xCurrentGR = juce::jlimit(x, x + width,
        positionForLevel(clampedDB));

    const int barStart = std::min(xNoReduction, xCurrentGR);
    const int barEnd = std::max(xNoReduction, xCurrentGR);

    if (barEnd > barStart)
    {
        g.setColour(fillColour);
        g.fillRect(barStart, y, barEnd - barStart, height);
    }
}

/**
 * @brief Convenience wrapper to draw the GR bar using the meter's GR colour.
 *
 * @param x               Left bar bound in pixels.
 * @param y               Top bar bound in pixels.
 * @param width           Bar width in pixels.
 * @param height          Bar height in pixels.
 * @param g               Graphics context to draw into.
 * @param levelDB         Current gain-reduction level in dB.
 * @param positionForLevel Callable mapping dB -> pixel X within [x, x + width].
 */
void GainReductionMeterLookAndFeel::drawRmsLevel(
    int x, int y, int width, int height,
    juce::Graphics& g,
    float levelDB,
    const std::function<int(float)>& positionForLevel) noexcept
{
    drawMeterBar(x, y, width, height, g, levelDB, positionForLevel,
        Colors::LevelMeter::gainReduction);
}


//==============================================================================
// MainLookAndFeel

MainLookAndFeel::MainLookAndFeel()
{
    setColour(juce::GroupComponent::textColourId, Colors::Group::label);
    setColour(juce::GroupComponent::outlineColourId, Colors::Group::outline);
}

juce::Font MainLookAndFeel::getLabelFont([[maybe_unused]] juce::Label& label)
{
    return Fonts::getFont();
}

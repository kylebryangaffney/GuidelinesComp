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

const float tickFontHeight = 8.0f;

// Loads and caches the LatoMedium font from binary data
const juce::Typeface::Ptr Fonts::typeface = juce::Typeface::createSystemTypefaceFor(
    BinaryData::LatoMedium_ttf, BinaryData::LatoMedium_ttfSize);

// Returns a styled font using the cached typeface
juce::Font Fonts::getFont(float height)
{
    return juce::FontOptions(typeface)
        .withMetricsKind(juce::TypefaceMetricsKind::legacy)
        .withHeight(height);
}

//================================================================================
// PresetPanelLookAndFeel

PresetPanelLookAndFeel::PresetPanelLookAndFeel()
{
    setColour(juce::ComboBox::backgroundColourId, Colors::PresetPanel::boxBackground);
    setColour(juce::ComboBox::outlineColourId, Colors::PresetPanel::outline);
    setColour(juce::ComboBox::textColourId, Colors::PresetPanel::text);

    setColour(juce::TextButton::buttonColourId, Colors::PresetPanel::buttonBase);
    setColour(juce::TextButton::textColourOnId, Colors::PresetPanel::text);
    setColour(juce::TextButton::textColourOffId, Colors::PresetPanel::text);

}

void PresetPanelLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& b,
    [[maybe_unused]] const juce::Colour& backgroundColor,
    bool isMouseOverButton, bool isButtonDown)
{
    auto bounds = b.getLocalBounds().toFloat();
    float corner = 4.0f;

    // Draw the main background
    g.setColour(b.findColour(juce::TextButton::buttonColourId));
    g.fillRoundedRectangle(bounds, corner);

    // Draw hover/pressed overlay (semi-transparent)
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

void PresetPanelLookAndFeel::drawButtonText(juce::Graphics& g,
    juce::TextButton& b,
    [[maybe_unused]] bool isMouseOverButton, [[maybe_unused]] bool isButtonDown)
{
    juce::Colour buttonColor = b.findColour(b.getToggleState() ? juce::TextButton::textColourOnId
        : juce::TextButton::textColourOffId);

    g.setColour(buttonColor);

    g.drawFittedText(b.getButtonText(), b.getLocalBounds(), juce::Justification::centred, 1);
}


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

void PresetPanelLookAndFeel::drawPresetPanelBackground(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(Colors::PresetPanel::background);
    g.fillRoundedRectangle(area.toFloat(), 7.0f);
}

//==============================================================================
// RotaryKnobLookAndFeel

// Constructor - sets default component colours for knobs
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

void RotaryKnobLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width,
    [[maybe_unused]] int height, float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle,
    juce::Slider& slider)
{
    // Define rotary drawing area and center
    juce::Rectangle<float> bounds = juce::Rectangle<float>(x, y, width, width);
    juce::Rectangle<float> knobRect = bounds.reduced(10.f);
    juce::Point<float> knobCenter = knobRect.getCentre();

    // Retrieve optional alert level (for peak/clipping color blending)
    auto* knob = dynamic_cast<RotaryKnob*>(slider.getParentComponent());
    float alertLevel = knob ? knob->getAlertLevel() : 0.0f;

    // Draw outer tick marks around the knob
    int numTicks = knob ? knob->getNumTicks() : 0;
    float tickRadius = knobRect.getWidth() / 2.0f + 2.0f;
    drawTicks(g, numTicks, knobCenter, tickRadius, rotaryStartAngle, rotaryEndAngle);

    // Draw main knob body with drop shadow and fill
    drawKnobBody(g, knobRect);

    // Fill interior with flat vertical gradient for matte effect
    auto innerRect = knobRect.reduced(2.f);
    juce::ColourGradient gradient(Colors::Knob::gradientTop, 0.f, innerRect.getY(),
        Colors::Knob::gradientBottom, 0.f, innerRect.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillEllipse(innerRect);

    // Subtle highlight ring on knob
    auto bevelRect = innerRect.reduced(1.5f);
    g.setColour(juce::Colours::white.withAlpha(0.08f));
    g.drawEllipse(bevelRect, 1.0f);

    // Draw arc track in background
    juce::Point<float> boundsCenter = bounds.getCentre();
    float radius = bounds.getWidth() / 2.f;
    float lineWidth = 3.f;
    float arcRadius = radius - lineWidth / 2.f;
    juce::PathStrokeType stroke(lineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

    drawArcTrack(g, bounds, boundsCenter, arcRadius, rotaryStartAngle, rotaryEndAngle, stroke);


    // Draw dial pointer to indicate current value
    float toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto dialRadius = innerRect.getHeight() / 2.f - lineWidth;
    drawDialIndicator(g, knobCenter, dialRadius, toAngle, lineWidth);


    // Draw active value arc based on position and alert level
    if (slider.isEnabled())
    {
        drawValueArc(g, slider, boundsCenter, stroke, rotaryStartAngle, rotaryEndAngle, arcRadius, toAngle, alertLevel);
    }
}

void RotaryKnobLookAndFeel::drawTicks(juce::Graphics& g,
    const int numTicks,
    const juce::Point<float> knobCenter,
    const float tickRadius,
    const float rotaryStartAngle,
    const float rotaryEndAngle) noexcept
{
    constexpr float tickLength = 6.0f;
    constexpr float tickThickness = 1.0f;

    // Preconditions (debug)
    jassert(numTicks >= 2);
    jassert(tickRadius > 0.0f);
    jassert(rotaryEndAngle != rotaryStartAngle);

    // Release guard
    if (numTicks < 2 || tickRadius <= 0.0f || rotaryEndAngle == rotaryStartAngle)
        return;

    const float denom = float(numTicks - 1);
    const float invDen = 1.0f / denom;
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


void RotaryKnobLookAndFeel::drawKnobBody(juce::Graphics& g, juce::Rectangle<float> knobRect)
{
    juce::Path knobShape;
    knobShape.addEllipse(knobRect);
    dropShadow.drawForPath(g, knobShape);

    g.setColour(Colors::Knob::outline);
    g.fillPath(knobShape);
}

void RotaryKnobLookAndFeel::drawArcTrack(juce::Graphics& g, juce::Rectangle<float> bounds, juce::Point<float> boundsCenter, float arcRadius, float rotaryStartAngle, float rotaryEndAngle, juce::PathStrokeType stroke)
{
    // Draw arc track in background
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(boundsCenter.x, boundsCenter.y, arcRadius, arcRadius, 0.f,
        rotaryStartAngle, rotaryEndAngle, true);

    g.setColour(Colors::Knob::trackBackground);
    g.strokePath(backgroundArc, stroke);
}


void RotaryKnobLookAndFeel::drawDialIndicator(juce::Graphics& g,
    juce::Point<float> knobCenter,
    float dialRadius,
    float toAngle,
    float lineWidth)
{

    juce::Point<float> dialStart(knobCenter.x + 2.f * std::sin(toAngle),
        knobCenter.y - 2.f * std::cos(toAngle));
    juce::Point<float> dialEnd(knobCenter.x + dialRadius * std::sin(toAngle),
        knobCenter.y - dialRadius * std::cos(toAngle));

    juce::Path dialPath;
    dialPath.startNewSubPath(dialStart);
    dialPath.lineTo(dialEnd);
    g.setColour(Colors::Knob::caret);
    g.strokePath(dialPath, juce::PathStrokeType(3.0f));

    juce::Rectangle<float> innerCircleRect(
        knobCenter.x - dialRadius,
        knobCenter.y - dialRadius,
        dialRadius * 2,
        dialRadius * 2);

    g.drawEllipse(innerCircleRect, lineWidth);
}

void RotaryKnobLookAndFeel::drawValueArc(juce::Graphics& g,
    juce::Slider& slider,
    juce::Point<float> boundsCenter,
    juce::PathStrokeType stroke,
    float rotaryStartAngle,
    float rotaryEndAngle,
    float arcRadius,
    float toAngle,
    float alertLevel
)
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

juce::Font RotaryKnobLookAndFeel::getLabelFont([[maybe_unused]] juce::Label& label)
{
    return Fonts::getFont();
}

void RotaryKnobLookAndFeel::fillTextEditorBackground(juce::Graphics& g,
    [[maybe_unused]] int width,
    [[maybe_unused]] int height,
    juce::TextEditor& textEditor)
{
    g.setColour(Colors::Knob::textBoxBackground);
    g.fillRoundedRectangle(textEditor.getLocalBounds().reduced(4, 0).toFloat(), 4.f);
}

class RotaryKnobLabel : public juce::Label
{
public:
    RotaryKnobLabel() = default;

    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails&) override {}

    std::unique_ptr<juce::AccessibilityHandler> createAccessibilityHandler() override
    {
        return createIgnoredAccessibilityHandler(*this);
    }

    juce::TextEditor* createEditorComponent() override
    {
        auto* ed = new juce::TextEditor(getName());
        ed->applyFontToAllText(getLookAndFeel().getLabelFont(*this));
        copyAllExplicitColoursTo(*ed);
        ed->setBorder(juce::BorderSize<int>());
        ed->setIndents(2, 1);
        ed->setJustification(juce::Justification::centredTop);
        ed->setPopupMenuEnabled(false);
        ed->setInputRestrictions(6);
        return ed;
    }
};

// Custom label for rotary slider value entry
juce::Label* RotaryKnobLookAndFeel::createSliderTextBox(juce::Slider& slider)
{
    auto* label = new RotaryKnobLabel();
    label->setJustificationType(juce::Justification::centred);
    label->setKeyboardType(juce::TextInputTarget::decimalKeyboard);
    label->setColour(juce::Label::textColourId,
        slider.findColour(juce::Slider::textBoxTextColourId));
    label->setColour(juce::TextEditor::textColourId, Colors::Knob::value);
    label->setColour(juce::TextEditor::highlightedTextColourId, Colors::Knob::value);
    label->setColour(juce::TextEditor::highlightColourId,
        slider.findColour(juce::Slider::rotarySliderFillColourId));
    label->setColour(juce::TextEditor::backgroundColourId, Colors::Knob::textBoxBackground);
    return label;
}

namespace
{
    void drawTickWithLabel(juce::Graphics& g, int x, int tickTop, int tickHeight,
        const juce::String& label, int labelTop, int labelWidth, int labelHeight)
    {
        g.setColour(Colors::LevelMeter::tickLine);
        g.fillRect(x, tickTop, 1, tickHeight);

        g.setColour(Colors::LevelMeter::tickLabel);
        g.drawText(
            label,
            x - labelWidth / 2, labelTop, labelWidth, labelHeight, juce::Justification::centred
        );
    }
}


//==============================================================================
// LevelMeterLookAndFeel
void LevelMeterLookAndFeel::drawLevelMeter(juce::Graphics& g, const LevelMeter& meter)
{
    const auto font = Fonts::getFont(tickFontHeight);
    constexpr float borderThickness = 1.0f;
    constexpr float borderRadius = 4.0f;

    auto borderRect = meter.getLocalBounds().toFloat().reduced(7.0f);
    auto innerRect = borderRect.reduced(10.0f);

    g.fillAll(Colors::LevelMeter::background);
    g.setColour(Colors::LevelMeter::border);
    g.drawRoundedRectangle(borderRect, borderRadius, borderThickness);
    g.setFont(font);

    // Bar settings
    constexpr int barHeight = 7;
    constexpr int rmsHeight = 4;
    constexpr int barSpacing = 2; // space between bars

    // Meter geometry (match your existing logic; keep conversions the same)
    const int xLeft = static_cast<int>(innerRect.getX());
    const int xRight = static_cast<int>(innerRect.getRight());
    const int w = static_cast<int>(innerRect.getWidth());

    // Early-out if degenerate
    if (w <= 0)
        return;

    // Bar Y positions
    const int yPeakL = static_cast<int>(innerRect.getY());
    const int yPeakR = yPeakL + barHeight + barSpacing;
    const int yRmsL = yPeakL;
    const int yRmsR = yPeakR;

    // Mapper: dB -> pixel x (explicit captures, no [&])
    const auto map = [xLeft, xRight, &meter](float db) noexcept
        {
            return meter.positionForLevel(db, xLeft, xRight);
        };

    // Draw peak and RMS meters for both channels
    drawPeakLevel(xLeft, yPeakL, w, barHeight, g, meter.getPeakLevelL(), map);
    drawPeakLevel(xLeft, yPeakR, w, barHeight, g, meter.getPeakLevelR(), map);

    drawRmsLevel(xLeft, yRmsL, w, rmsHeight, g, meter.getRmsLevelL(), map);
    drawRmsLevel(xLeft, yRmsR, w, rmsHeight, g, meter.getRmsLevelR(), map);

    // Tick marks and labels
    const int tickTop = yPeakL + 2;          // 2 px down from top
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

// Draws a single meter bar (RMS or peak)
void LevelMeterLookAndFeel::drawMeterBar(int x, int y, int width, int height, juce::Graphics& g, float levelDB,
    std::function<int(float)> positionForLevel, juce::Colour fillColour)
{
    if (levelDB <= clampdB)
        return;

    const auto clampToBar = [&](float db)
        {
            return juce::jlimit(x, x + width, positionForLevel(db));
        };
    const int xStart = clampToBar(levelDB);
    const int xZero = clampToBar(0.0f);

    if (xStart > xZero)
    {
        g.setColour(fillColour);
        g.fillRect(x, y, xZero - x, height);

        g.setColour(Colors::LevelMeter::tooLoud);
        g.fillRect(xZero, y, xStart - xZero, height);
    }
    else
    {
        g.setColour(fillColour);
        g.fillRect(x, y, xStart - x, height);
    }
}

// Wrapper for drawing left/right peak levels
void LevelMeterLookAndFeel::drawPeakLevel(int x, int y, int width, int height,
    juce::Graphics& g,
    float levelDB,
    std::function<int(float)> positionForLevel)
{
    drawMeterBar(x, y, width, height, g, levelDB, positionForLevel, Colors::LevelMeter::peakLevelOK);
}
// Wrapper for drawing left/right RMS levels  rmsLevelOK
void LevelMeterLookAndFeel::drawRmsLevel(int x, int y, int width, int height,
    juce::Graphics& g,
    float levelDB,
    std::function<int(float)> positionForLevel)
{
    drawMeterBar(x, y, width, height, g, levelDB, positionForLevel, Colors::LevelMeter::rmsLevelOK);
}


//=================================
// GainReductionMeterLookAndFeel
void GainReductionMeterLookAndFeel::drawGainReductionMeter(juce::Graphics& g, const GainReductionMeter& meter)
{
    auto font = Fonts::getFont(tickFontHeight);
    constexpr float borderThickness = 1.0f;
    constexpr float borderRadius = 4.0f;

    auto borderRect = meter.getLocalBounds().toFloat().reduced(7.0f);
    auto innerRect = borderRect.reduced(10.0f);

    g.fillAll(Colors::LevelMeter::background);

    g.setColour(Colors::LevelMeter::border);
    g.drawRoundedRectangle(borderRect, borderRadius, borderThickness);
    g.setFont(font);


    // Bar settings
    const int barHeight = 7;
    const int rmsHeight = 4;
    // Draw RMS bar, using same wrappers as LevelMeter
    drawRmsLevel(innerRect.getX(), innerRect.getY(), innerRect.getWidth(), barHeight, g, meter.getMaxRmsLevel(),
        [&](float db) { return meter.positionForLevel(db, innerRect.getX(), innerRect.getRight()); });

    // Draw dB tick marks with numeric labels
    const int tickTop = innerRect.getY() + 2;           // 2 px down from top
    const int tickHeight = 8;
    const int labelTop = tickTop + tickHeight;        // below tick
    const int labelHeight = 12;
    constexpr int labelWidth = 27;


    // Draw vertical ticks and labels
    for (float db = GainReductionMeter::maxdB; db >= GainReductionMeter::mindB - 0.1f; db -= GainReductionMeter::stepdB)
    {
        int x = meter.positionForLevel(db, innerRect.getX(), innerRect.getRight());
        drawTickWithLabel(g, x, tickTop, tickHeight, juce::String(int(db)), labelTop, labelWidth, labelHeight);
    }
}
void GainReductionMeterLookAndFeel::drawMeterBar(
    int x, int y, int width, int height, juce::Graphics& g, float levelDB,
    std::function<int(float)> positionForLevel, juce::Colour fillColour)
{
    if (width <= 0 || height <= 0)
        return;

    const float clampedDB = juce::jlimit(GainReductionMeter::mindB, GainReductionMeter::maxdB, levelDB);

    const int a = juce::jlimit(x, x + width, positionForLevel(GainReductionMeter::maxdB)); // “no GR”
    const int b = juce::jlimit(x, x + width, positionForLevel(clampedDB));                 // current GR

    const int barStart = std::min(a, b);
    const int barEnd = std::max(a, b);

    if (barEnd > barStart)
    {
        g.setColour(fillColour);
        g.fillRect(barStart, y, barEnd - barStart, height);
    }
}


// Wrapper for drawing left/right RMS levels
void GainReductionMeterLookAndFeel::drawRmsLevel(
    int x, int y, int width, int height,
    juce::Graphics& g,
    float levelDB,
    std::function<int(float)> positionForLevel)
{
    drawMeterBar(x, y, width, height, g, levelDB, positionForLevel, Colors::LevelMeter::gainReduction);
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

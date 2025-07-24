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

void PresetPanelLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& b, const juce::Colour& backgroundColor,
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
    bool isMouseOverButton, bool isButtonDown)
{
    if (b.getButtonText() == juce::String::charToString(0x2714) || // Checkmark
        b.getButtonText() == juce::String::charToString(0x2716))   // X
    {
        return;
    }

    LookAndFeel_V4::drawButtonText(g, b, false, false);
}


void PresetPanelLookAndFeel::drawComboBox(juce::Graphics& g,
    int width, int height, bool isButtonDown,
    int buttonX, int buttonY, int buttonW, int buttonH,
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
    int numTicks,
    juce::Point<float> knobCenter,
    float tickRadius,
    float rotaryStartAngle,
    float rotaryEndAngle)
{
    const float tickLength = 7.0f;
    const float tickThickness = 1.0f;

    for (int i = 0; i < numTicks; ++i)
    {
        float angle = rotaryStartAngle + i * (rotaryEndAngle - rotaryStartAngle) / (numTicks - 1);
        auto p1 = knobCenter.getPointOnCircumference(tickRadius, angle);
        auto p2 = knobCenter.getPointOnCircumference(tickRadius + tickLength, angle);

        g.setColour(Colors::Knob::tick);
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

//==============================================================================
// LevelMeterLookAndFeel

void LevelMeterLookAndFeel::drawLevelMeter(juce::Graphics& g, const LevelMeter& meter)
{
    constexpr float borderThickness = 1.0f;
    constexpr float borderRadius = 4.0f;

    auto borderRect = meter.getLocalBounds().toFloat().reduced(7.0f);
    auto innerRect = borderRect.reduced(10.0f);

    g.fillAll(Colors::LevelMeter::background);

    g.setColour(Colors::LevelMeter::border);
    g.drawRoundedRectangle(borderRect, borderRadius, borderThickness);
    g.setFont(Fonts::getFont(tickFontHeight));

    // Bar settings
    const int barHeight = 7;
    const int rmsHeight = 4;
    const int barSpacing = 2; // space between bars

    // Calculate bar Y positions within 'inner'
    int yPeakL = innerRect.getY();
    int yPeakR = yPeakL + barHeight + barSpacing;
    int yRmsL = yPeakL;
    int yRmsR = yPeakR;

    // Draw peak and RMS meters for both channels
    drawPeakLevel(innerRect.getX(), yPeakL, innerRect.getWidth(), barHeight, g, meter.getPeakLevelL(),
        [&](float db) { return meter.positionForLevel(db, innerRect.getX(), innerRect.getRight()); });
    drawPeakLevel(innerRect.getX(), yPeakR, innerRect.getWidth(), barHeight, g, meter.getPeakLevelR(),
        [&](float db) { return meter.positionForLevel(db, innerRect.getX(), innerRect.getRight()); });

    drawRmsLevel(innerRect.getX(), yRmsL, innerRect.getWidth(), rmsHeight, g, meter.getRmsLevelL(),
        [&](float db) { return meter.positionForLevel(db, innerRect.getX(), innerRect.getRight()); });
    drawRmsLevel(innerRect.getX(), yRmsR, innerRect.getWidth(), rmsHeight, g, meter.getRmsLevelR(),
        [&](float db) { return meter.positionForLevel(db, innerRect.getX(), innerRect.getRight()); });

    // Draw dB tick marks with numeric labels
    const int tickTop = innerRect.getY() + 2;           // 2 px down from top
    const int tickHeight = 14;
    const int labelTop = tickTop + tickHeight;        // below tick
    const int labelHeight = 12;

    for (float db = meter.maxdB; db >= meter.mindB; db -= meter.stepdB)
    {
        int x = meter.positionForLevel(db, innerRect.getX(), innerRect.getRight());
        g.setColour(Colors::LevelMeter::tickLine);
        g.fillRect(x, tickTop, 1, tickHeight);

        g.setColour(Colors::LevelMeter::tickLabel);
        g.drawText(
            juce::String(int(db)),
            x - 12,
            labelTop,
            24,
            labelHeight,
            juce::Justification::centred
        );
    }
}

// Draws a single meter bar (RMS or peak)
void LevelMeterLookAndFeel::drawMeterBar(int x, int y, int width, int height, juce::Graphics& g, float levelDB,
    std::function<int(float)> positionForLevel, juce::Colour fillColour)
{
    if (levelDB <= clampdB)
        return;

    const int xStart = juce::jlimit(x, x + width, positionForLevel(levelDB));
    const int xZero = juce::jlimit(x, x + width, positionForLevel(0.0f));

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
    constexpr float borderThickness = 1.0f;
    constexpr float borderRadius = 4.0f;

    auto borderRect = meter.getLocalBounds().toFloat().reduced(7.0f);
    auto innerRect = borderRect.reduced(10.0f);

    g.fillAll(Colors::LevelMeter::background);

    g.setColour(Colors::LevelMeter::border);
    g.drawRoundedRectangle(borderRect, borderRadius, borderThickness);
    g.setFont(Fonts::getFont(tickFontHeight));


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

    // Draw vertical ticks and labels
    for (float db = GainReductionMeter::maxdB; db >= GainReductionMeter::mindB - 0.1f; db -= GainReductionMeter::stepdB)
    {
        int x = meter.positionForLevel(db, innerRect.getX(), innerRect.getRight());
        g.setColour(Colors::LevelMeter::tickLine);
        g.fillRect(x, tickTop, 1, tickHeight);

        g.setColour(Colors::LevelMeter::tickLabel);
        g.drawText(
            juce::String(int(db)),
            x - 8, labelTop, 28, labelHeight, juce::Justification::centred
        );
    }
}

// Horizontal bar
void GainReductionMeterLookAndFeel::drawMeterBar(
    int x, int y, int width, int height, juce::Graphics& g, float levelDB,
    std::function<int(float)> positionForLevel, juce::Colour fillColour)
{
    float clampedDB = juce::jlimit(GainReductionMeter::mindB, GainReductionMeter::maxdB, levelDB);

    int xRight = juce::jlimit(x, x + width, positionForLevel(GainReductionMeter::maxdB));  // no reduction (right)
    int xEnd = juce::jlimit(x, x + width, positionForLevel(clampedDB));                  // current reduction

    // Draw gain reduction bar
    if (xEnd < xRight)
    {
        g.setColour(fillColour);
        g.fillRect(xEnd, y, xRight - xEnd, height);
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

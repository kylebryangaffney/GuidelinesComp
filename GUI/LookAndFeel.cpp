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
    const int numTicks = 21;
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
    const auto bounds = meter.getLocalBounds();
    g.fillAll(Colors::LevelMeter::background);
    g.setFont(Fonts::getFont(10.f));

    // Draw peak and RMS meters for both left and right channels
    drawPeakLevel(g, meter.getPeakLevelL(), 0, 7,
        [&](float db) { return meter.positionForLevel(db); },
        meter.getHeight());

    drawPeakLevel(g, meter.getPeakLevelR(), 9, 7,
        [&](float db) { return meter.positionForLevel(db); },
        meter.getHeight());

    drawRmsLevel(g, meter.getRmsLevelL(), 1, 4,
        [&](float db) { return meter.positionForLevel(db); },
        meter.getHeight());

    drawRmsLevel(g, meter.getRmsLevelR(), 10, 4,
        [&](float db) { return meter.positionForLevel(db); },
        meter.getHeight());

    // Draw dB tick marks with numeric labels
    for (float db = meter.maxdB; db >= meter.mindB; db -= meter.stepdB)
    {
        int y = meter.positionForLevel(db);
        g.setColour(Colors::LevelMeter::tickLine);
        g.fillRect(0, y, 16, 1);

        g.setColour(Colors::LevelMeter::tickLabel);
        g.drawSingleLineText(juce::String(int(db)),
            bounds.getWidth(),
            y + 3,
            juce::Justification::right);
    }
}

// Draws a single meter bar (RMS or peak)
void LevelMeterLookAndFeel::drawMeterBar(juce::Graphics& g, float levelDB, int x, int width,
    juce::Colour fillColour,
    std::function<int(float)> positionForLevel,
    int height)
{
    if (levelDB <= clampdB)
        return;

    const int yStart = juce::jlimit(0, height, positionForLevel(levelDB));
    const int yZero = juce::jlimit(0, height, positionForLevel(0.0f));

    if (yStart < yZero)
    {
        g.setColour(Colors::LevelMeter::tooLoud);
        g.fillRect(x, yStart, width, yZero - yStart);

        g.setColour(fillColour);
        g.fillRect(x, yZero, width, height - yZero);
    }
    else
    {
        g.setColour(fillColour);
        g.fillRect(x, yStart, width, height - yStart);
    }
}

// Wrapper for drawing left/right peak levels
void LevelMeterLookAndFeel::drawPeakLevel(juce::Graphics& g, float level, int x, int width,
    std::function<int(float)> positionForLevel, int height)
{
    drawMeterBar(g, level, x, width, Colors::LevelMeter::peakLevelOK,
        positionForLevel, height);
}

// Wrapper for drawing left/right RMS levels
void LevelMeterLookAndFeel::drawRmsLevel(juce::Graphics& g, float level, int x, int width,
    std::function<int(float)> positionForLevel, int height)
{
    drawMeterBar(g, level, x, width, Colors::LevelMeter::rmsLevelOK,
        positionForLevel, height);
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

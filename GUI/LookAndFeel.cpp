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
const juce::Typeface::Ptr Fonts::typeface = juce::Typeface::createSystemTypefaceFor(
    BinaryData::LatoMedium_ttf, BinaryData::LatoMedium_ttfSize);

juce::Font Fonts::getFont(float height)
{
    return juce::FontOptions(typeface)
        .withMetricsKind(juce::TypefaceMetricsKind::legacy)
        .withHeight(height);
}

//==============================================================================
// RotaryKnobLookAndFeel
RotaryKnobLookAndFeel::RotaryKnobLookAndFeel()
{
    setColour(juce::Label::textColourId, Colors::Knob::label);
    setColour(juce::Slider::textBoxTextColourId, Colors::Knob::label);
    setColour(juce::Slider::rotarySliderFillColourId, Colors::Slider::sliderFill);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::CaretComponent::caretColourId, Colors::Knob::caret);
}

void RotaryKnobLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width,
    [[maybe_unused]] int height, float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle,
    juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float>(x, y, width, width);
    auto knobRect = bounds.reduced(10.f);
    auto knobCenter = knobRect.getCentre();

    auto* knob = dynamic_cast<RotaryKnob*>(slider.getParentComponent());
    float alertLevel = knob ? knob->getAlertLevel() : 0.0f;

    // --- Teeth (drawn inward from outer radius)
    const int numTeeth = 24;
    const float toothDepth = 8.0f;
    float outerRadius = knobRect.getWidth() / 2.0f;
    float innerRadius = outerRadius - toothDepth;

    juce::Path teethPath;
    for (int i = 0; i < numTeeth; ++i)
    {
        float angle = juce::MathConstants<float>::twoPi * i / numTeeth;

        auto outer = juce::Point<float>(
            knobCenter.x + outerRadius * std::cos(angle),
            knobCenter.y + outerRadius * std::sin(angle));

        auto inner = juce::Point<float>(
            knobCenter.x + innerRadius * std::cos(angle),
            knobCenter.y + innerRadius * std::sin(angle));

        teethPath.startNewSubPath(outer);
        teethPath.lineTo(inner);
    }

    g.setColour(Colors::Knob::outline.withAlpha(0.4f));
    g.strokePath(teethPath, juce::PathStrokeType(1.2f));

    // --- Main knob body
    juce::Path knobShape;
    knobShape.addEllipse(knobRect);
    dropShadow.drawForPath(g, knobShape);

    g.setColour(Colors::Knob::outline);
    g.fillPath(knobShape);

    // --- Radial lighting gradient
    auto innerRect = knobRect.reduced(2.f);
    auto centerPoint = innerRect.getCentre();
    juce::ColourGradient radialGradient(
        Colors::Knob::gradientTop, centerPoint.x, centerPoint.y,
        Colors::Knob::gradientBottom.darker(0.3f), centerPoint.x, centerPoint.y + innerRect.getHeight() * 0.6f,
        true
    );
    g.setGradientFill(radialGradient);
    g.fillEllipse(innerRect);

    // --- Bevel rim
    auto bevelRect = innerRect.reduced(1.5f);
    g.setColour(juce::Colours::white.withAlpha(0.08f));
    g.drawEllipse(bevelRect, 1.0f);

    // --- Arc background
    auto boundsCenter = bounds.getCentre();
    auto radius = bounds.getWidth() / 2.f;
    auto lineWidth = 3.f;
    auto arcRadius = radius - lineWidth / 2.f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(boundsCenter.x, boundsCenter.y, arcRadius, arcRadius, 0.f,
        rotaryStartAngle, rotaryEndAngle, true);

    juce::PathStrokeType stroke(lineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    g.setColour(Colors::Knob::trackBackground);
    g.strokePath(backgroundArc, stroke);

    // --- Dial indicator line
    float toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto dialRadius = innerRect.getHeight() / 2.f - lineWidth;

    juce::Point<float> dialStart(boundsCenter.x + 8.f * std::sin(toAngle),
        boundsCenter.y - 8.f * std::cos(toAngle));
    juce::Point<float> dialEnd(boundsCenter.x + dialRadius * std::sin(toAngle),
        boundsCenter.y - dialRadius * std::cos(toAngle));

    juce::Path dialPath;
    dialPath.startNewSubPath(dialStart);
    dialPath.lineTo(dialEnd);
    g.setColour(Colors::Knob::dial);
    g.strokePath(dialPath, stroke);

    // --- Active value arc
    if (slider.isEnabled())
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
}


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


//======================================
// LevelMeterLookAndFeel

void LevelMeterLookAndFeel::drawLevelMeter(juce::Graphics& g, const LevelMeter& meter)
{
    const auto bounds = meter.getLocalBounds();
    g.fillAll(Colors::LevelMeter::background);
    g.setFont(Fonts::getFont(10.f));

    // Draw LEFT peak
    drawPeakLevel(g, meter.getPeakLevelL(), 0, 7,
        [&](float db) { return meter.positionForLevel(db); },
        meter.getHeight());

    // RIGHT peak
    drawPeakLevel(g, meter.getPeakLevelR(), 9, 7,
        [&](float db) { return meter.positionForLevel(db); },
        meter.getHeight());

    // LEFT RMS
    drawRmsLevel(g, meter.getRmsLevelL(), 1, 4,
        [&](float db) { return meter.positionForLevel(db); },
        meter.getHeight());

    // RIGHT RMS
    drawRmsLevel(g, meter.getRmsLevelR(), 10, 4,
        [&](float db) { return meter.positionForLevel(db); },
        meter.getHeight());

    // Ticks and labels
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

void LevelMeterLookAndFeel::drawPeakLevel(juce::Graphics& g, float level, int x, int width,
    std::function<int(float)> positionForLevel, int height)
{
    drawMeterBar(g, level, x, width, Colors::LevelMeter::peakLevelOK,
        positionForLevel, height);
}

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

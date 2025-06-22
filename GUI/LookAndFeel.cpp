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

    auto* knob = dynamic_cast<RotaryKnob*>(slider.getParentComponent());
    float alertLevel = knob ? knob->getAlertLevel() : 0.0f;

    juce::Path knobShape;
    knobShape.addEllipse(knobRect);
    dropShadow.drawForPath(g, knobShape);

    g.setColour(Colors::Knob::outline);
    g.fillEllipse(knobRect);

    auto innerRect = knobRect.reduced(2.f);
    juce::ColourGradient gradient(Colors::Knob::gradientTop, 0.f, innerRect.getY(),
        Colors::Knob::gradientBottom, 0.f, innerRect.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillEllipse(innerRect);

    auto center = bounds.getCentre();
    auto radius = bounds.getWidth() / 2.f;
    auto lineWidth = 3.f;
    auto arcRadius = radius - lineWidth / 2.f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(center.x, center.y, arcRadius, arcRadius, 0.f,
        rotaryStartAngle, rotaryEndAngle, true);

    juce::PathStrokeType stroke(lineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    g.setColour(Colors::Knob::trackBackground);
    g.strokePath(backgroundArc, stroke);

    float toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto dialRadius = innerRect.getHeight() / 2.f - lineWidth;

    juce::Point<float> dialStart(center.x + 8.f * std::sin(toAngle),
        center.y - 8.f * std::cos(toAngle));
    juce::Point<float> dialEnd(center.x + dialRadius * std::sin(toAngle),
        center.y - dialRadius * std::cos(toAngle));

    juce::Path dialPath;
    dialPath.startNewSubPath(dialStart);
    dialPath.lineTo(dialEnd);
    g.setColour(Colors::Knob::dial);
    g.strokePath(dialPath, stroke);

    if (slider.isEnabled())
    {
        float fromAngle = rotaryStartAngle;
        if (slider.getProperties()["drawFromMiddle"])
            fromAngle += (rotaryEndAngle - rotaryStartAngle) / 2.f;

        juce::Path valueArc;
        valueArc.addCentredArc(center.x, center.y, arcRadius, arcRadius, 0.f,
            fromAngle, toAngle, true);

        juce::Colour finalFillColor = Colors::Slider::sliderFill.interpolatedWith(Colors::Slider::sliderClippingFill, alertLevel);

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

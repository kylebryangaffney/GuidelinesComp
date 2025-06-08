/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/


/// add more space between knobs, groups, and meters. posibly make it all taller as well 
/// 330, 450 is a good ratio, but needs to be bigger 



#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuideLinesCompAudioProcessorEditor::GuideLinesCompAudioProcessorEditor(GuideLinesCompAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&mainLF);

    lowCutKnob = std::make_unique<RotaryKnob>("Low Cut", p.apvts, lowCutParamID);
    controlKnob = std::make_unique<RotaryKnob>("Control", p.apvts, controlParamID);
    compressionKnob = std::make_unique<RotaryKnob>("Compress", p.apvts, compressionParamID);
    outputGainKnob = std::make_unique<AsymmetricalRotaryKnob>("Output", p.apvts, outputGainParamID);
    inputMeter = std::make_unique<LevelMeter>(
        p.peakInputLevelLeft, p.peakInputLevelRight,
        p.rmsInputLevelLeft, p.rmsInputLevelRight);

    outputMeter = std::make_unique<LevelMeter>(
        p.peakOutputLevelLeft, p.peakOutputLevelRight,
        p.rmsOutputLevelLeft, p.rmsOutputLevelRight);

    // Control group
    //controlGroup.setText("Controls");
    //controlGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    controlGroup.addAndMakeVisible(*lowCutKnob);
    controlGroup.addAndMakeVisible(*controlKnob);
    controlGroup.addAndMakeVisible(*compressionKnob);
    addAndMakeVisible(controlGroup);

    // Output group
    //outputGroup.setText("Output");
    //outputGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    outputGroup.addAndMakeVisible(*outputGainKnob);
    outputGroup.addAndMakeVisible(*outputMeter);
    addAndMakeVisible(outputGroup);

    meterGroup.setText("Meters");
    meterGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    meterGroup.addAndMakeVisible(*inputMeter);
    addAndMakeVisible(meterGroup);


    setSize(330, 450);
}

GuideLinesCompAudioProcessorEditor::~GuideLinesCompAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void GuideLinesCompAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(Colors::background);

    auto rect = getLocalBounds().withHeight(40);
    g.setColour(Colors::header);
    g.fillRect(rect);
}

void GuideLinesCompAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    int headerHeight = 50;
    int presetPanelHeight = 40;
    int padding = 10;
    int y = headerHeight + presetPanelHeight + padding;
    const int controlAndMeterWidth = 220;
    const int outputWidth = 80;
    constexpr int knobWidth = 60;
    constexpr int knobHeight = 100;
    constexpr int knobSpacing = 10;

    controlGroup.setBounds(padding, y, controlAndMeterWidth, 225 - y - padding);
    auto controlGroupArea = controlGroup.getLocalBounds().reduced(padding);
    int knobX = controlGroupArea.getX();
    int knobY = controlGroupArea.getY();

    lowCutKnob->setTopLeftPosition(knobX, knobY);
    controlKnob->setTopLeftPosition(lowCutKnob->getRight() + padding, knobY);
    compressionKnob->setTopLeftPosition(controlKnob->getRight() + padding, knobY);

    outputGroup.setBounds(controlGroup.getRight() + padding, y, outputWidth, bounds.getHeight() - y - padding);
    auto outputGroupArea = outputGroup.getLocalBounds().reduced(padding);
    outputGainKnob->setTopLeftPosition(knobX, knobY);

    outputMeter->setBounds(outputGroup.getWidth() - 45, 30, 30, outputGainKnob->getBottom() - 30);

    meterGroup.setBounds(10, controlGroup.getBottom() + padding, controlAndMeterWidth, 225 - padding);
}

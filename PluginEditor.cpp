/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuideLinesCompAudioProcessorEditor::GuideLinesCompAudioProcessorEditor(GuideLinesCompAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&mainLF);

    lowCutKnob = std::make_unique<RotaryKnob>("Low Cut", p.apvts, lowCutParamID);
    controlKnob = std::make_unique<RotaryKnob>("Control", p.apvts, controlParamID);
    compressionKnob = std::make_unique<RotaryKnob>("Compression", p.apvts, compressionParamID);

    controlGroup.addAndMakeVisible(*lowCutKnob);
    controlGroup.addAndMakeVisible(*controlKnob);
    controlGroup.addAndMakeVisible(*compressionKnob);

    // Control group
    controlGroup.setText("Controls");
    controlGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);

    addAndMakeVisible(controlGroup);


    setSize(600, 500);
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
    int padding = 12;
    int y = headerHeight + presetPanelHeight + padding;

    controlGroup.setBounds(10, y, 225, 235 - y - padding);

    auto knobArea = controlGroup.getLocalBounds().reduced(10);

    constexpr int knobWidth = 60;
    constexpr int knobHeight = 100;
    constexpr int knobSpacing = 10;

    int controlX = knobArea.getX();
    int controlY = knobArea.getY();

    lowCutKnob->setBounds(controlX, controlY, knobWidth, knobHeight);
    controlX += knobWidth + knobSpacing;

    controlKnob->setBounds(controlX, controlY, knobWidth, knobHeight);
    controlX += knobWidth + knobSpacing;

    compressionKnob->setBounds(controlX, controlY, knobWidth, knobHeight);
}

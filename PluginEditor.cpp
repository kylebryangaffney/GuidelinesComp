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

    addAndMakeVisible(lowCutKnob);

    // Compress group
    compressGroup.setText("Compress");
    compressGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    compressGroup.addAndMakeVisible(compressionKnob);
    compressGroup.addAndMakeVisible(inputMeter);
    addAndMakeVisible(compressGroup);

    // Control group
    controlGroup.setText("Control");
    controlGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    controlGroup.addAndMakeVisible(controlKnob);
    controlGroup.addAndMakeVisible(gRMeter);
    addAndMakeVisible(controlGroup);

    // Output group
    outputGroup.setText("Output");
    outputGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    outputGroup.addAndMakeVisible(outputGainKnob);
    outputGroup.addAndMakeVisible(outputMeter);
    addAndMakeVisible(outputGroup);

    setSize(400, 430);
    startTimerHz(60);
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
    int padding = 15;
    int y = headerHeight + presetPanelHeight + padding;

    int groupWidth = 83;
    int groupHeight = bounds.getHeight() - y - padding;
    int groupSpacing = padding;

    lowCutKnob.setTopLeftPosition(padding, y + padding);

    compressGroup.setBounds(lowCutKnob.getRight() + groupSpacing, y, groupWidth, groupHeight);
    controlGroup.setBounds(compressGroup.getRight() + groupSpacing, y, groupWidth, groupHeight);
    outputGroup.setBounds(controlGroup.getRight() + groupSpacing, y, groupWidth, groupHeight);

    auto compressArea = compressGroup.getLocalBounds().reduced(padding);
    compressionKnob.setTopLeftPosition(compressArea.getX(), compressArea.getY());
    inputMeter.setBounds(compressArea.getX(), compressionKnob.getBottom() + padding, groupWidth - 2 * padding, 160);

    auto controlArea = controlGroup.getLocalBounds().reduced(padding);
    controlKnob.setTopLeftPosition(controlArea.getX(), controlArea.getY());
    gRMeter.setBounds(controlArea.getX(), controlKnob.getBottom() + padding, groupWidth - 2 * padding, 160);

    auto outputArea = outputGroup.getLocalBounds().reduced(padding);
    outputGainKnob.setTopLeftPosition(outputArea.getX(), outputArea.getY());
    outputMeter.setBounds(outputArea.getX(), outputGainKnob.getBottom() + padding, groupWidth - 2 * padding, 160);
}


void GuideLinesCompAudioProcessorEditor::timerCallback()
{
    auto input = audioProcessor.getPeakInputLevelForKnob();
    auto compress = audioProcessor.getCompressionAmountForKnob();
    auto output = audioProcessor.getPeakOutputLevelForKnob();

    compressionKnob.setAlertLevel(juce::jlimit(0.f, 1.f, input));
    controlKnob.setAlertLevel(juce::jlimit(0.f, 1.f, compress / 12));
    outputGainKnob.setAlertLevel(juce::jlimit(0.f, 1.f, output));
}

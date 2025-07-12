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
    //compressGroup.setText("Compress");
    //compressGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    compressGroup.addAndMakeVisible(compressionKnob);
    compressGroup.addAndMakeVisible(inputMeter);
    addAndMakeVisible(compressGroup);

    // Control group
    //controlGroup.setText("Control");
    //controlGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    controlGroup.addAndMakeVisible(controlKnob);
    controlGroup.addAndMakeVisible(gRMeter);
    addAndMakeVisible(controlGroup);

    // Output group
    //outputGroup.setText("Output");
    //outputGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    outputGroup.addAndMakeVisible(outputGainKnob);
    outputGroup.addAndMakeVisible(outputMeter);
    addAndMakeVisible(outputGroup);

    setSize(450, 700);
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
    int knobWidth = 60;
    int padding = 15;
    int y = headerHeight + presetPanelHeight + padding;

    int groupWidth = bounds.getWidth() - 3 * padding;
    int groupHeight = 130;
    int meterWidth = groupWidth - knobWidth - (6 * padding);

    lowCutKnob.setTopLeftPosition(padding, y + padding);

    compressGroup.setBounds(padding, lowCutKnob.getBottom() + padding, groupWidth, groupHeight);
    controlGroup.setBounds(padding, compressGroup.getBottom() + padding, groupWidth, groupHeight);
    outputGroup.setBounds(padding, controlGroup.getBottom() + padding, groupWidth, groupHeight);

    auto compressArea = compressGroup.getLocalBounds().reduced(padding);
    compressionKnob.setTopLeftPosition(compressArea.getX(), compressArea.getY());
    inputMeter.setBounds(compressionKnob.getRight() + padding, compressionKnob.getY() + 2 * padding, meterWidth, 60);

    auto controlArea = controlGroup.getLocalBounds().reduced(padding);
    controlKnob.setTopLeftPosition(controlArea.getX(), controlArea.getY());
    gRMeter.setBounds(controlKnob.getRight() + padding, controlKnob.getY() + 2 * padding, meterWidth, 60);

    auto outputArea = outputGroup.getLocalBounds().reduced(padding);
    outputGainKnob.setTopLeftPosition(outputArea.getX(), outputArea.getY());
    outputMeter.setBounds(outputGainKnob.getRight() + padding, outputGainKnob.getY() + 2 * padding, meterWidth, 60);
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

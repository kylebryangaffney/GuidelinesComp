/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

float getNormalizedAlertLevel(float value, float startThreshold, float endThreshold)
{
    return juce::jlimit(0.0f, 1.0f, (value - startThreshold) / (endThreshold - startThreshold));
}


//==============================================================================
GuideLinesCompAudioProcessorEditor::GuideLinesCompAudioProcessorEditor(GuideLinesCompAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), presetPanel(p.getPresetManager())
{
    setLookAndFeel(&mainLF);

    addAndMakeVisible(presetPanel);

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

    setSize(400, 550);
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
    int knobWidth = 57;
    int padding = 7;
    int y = headerHeight + padding;

    int groupWidth = bounds.getWidth() - 3 * padding;
    int groupHeight = 105;
    int meterWidth = groupWidth - knobWidth - (3 * padding);
    int meterHeight = 55;

    lowCutKnob.setTopLeftPosition(padding, y + padding);

    presetPanel.setBounds(lowCutKnob.getRight() + (groupWidth / 2), lowCutKnob.getY(), groupWidth / 3, groupHeight / 1.5);

    compressGroup.setBounds(padding, lowCutKnob.getBottom() + padding, groupWidth, groupHeight);
    controlGroup.setBounds(padding, compressGroup.getBottom() + padding, groupWidth, groupHeight);
    outputGroup.setBounds(padding, controlGroup.getBottom() + padding, groupWidth, groupHeight);

    auto compressArea = compressGroup.getLocalBounds().reduced(padding);
    compressionKnob.setTopLeftPosition(compressArea.getX(), compressArea.getY());
    inputMeter.setBounds(compressionKnob.getRight() + padding, compressionKnob.getY() + 2 * padding, meterWidth, meterHeight);

    auto controlArea = controlGroup.getLocalBounds().reduced(padding);
    controlKnob.setTopLeftPosition(controlArea.getX(), controlArea.getY());
    gRMeter.setBounds(controlKnob.getRight() + padding, controlKnob.getY() + 2 * padding, meterWidth, meterHeight);

    auto outputArea = outputGroup.getLocalBounds().reduced(padding);
    outputGainKnob.setTopLeftPosition(outputArea.getX(), outputArea.getY());
    outputMeter.setBounds(outputGainKnob.getRight() + padding, outputGainKnob.getY() + 2 * padding, meterWidth, meterHeight);
}


void GuideLinesCompAudioProcessorEditor::timerCallback()
{
    auto input = audioProcessor.getPeakInputLevelForKnob();
    auto compress = audioProcessor.getCompressionAmountForKnob();
    auto output = audioProcessor.getPeakOutputLevelForKnob();

    compressionKnob.setAlertLevel(getNormalizedAlertLevel(input, 0.8f, 1.2f));
    controlKnob.setAlertLevel(juce::jlimit(0.f, 1.f, compress / 12));
    outputGainKnob.setAlertLevel(getNormalizedAlertLevel(output, 0.8f, 1.2f));

    //compressionKnob.setAlertLevel(juce::jlimit(0.0f, 1.0f, input));
    //controlKnob.setAlertLevel(juce::jlimit(0.f, 1.f, compress / 12));
    //outputGainKnob.setAlertLevel(juce::jlimit(0.0f, 1.0f, output));

}
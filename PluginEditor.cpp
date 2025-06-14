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

    // Control group
    //controlGroup.setText("Controls");
    //controlGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    controlGroup.addAndMakeVisible(lowCutKnob);
    controlGroup.addAndMakeVisible(compressionKnob);
    controlGroup.addAndMakeVisible(controlKnob);
    addAndMakeVisible(controlGroup);

    // Output group
    //outputGroup.setText("Output");
    //outputGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    outputGroup.addAndMakeVisible(outputGainKnob);
    outputGroup.addAndMakeVisible(outputMeter);
    addAndMakeVisible(outputGroup);

    meterGroup.setText("Meters");
    meterGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    meterGroup.addAndMakeVisible(inputMeter);
    meterGroup, addAndMakeVisible(gRMeter);
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
    const int meterWidth = 45;
    const int meterHeight = 160;

    controlGroup.setBounds(padding, y, controlAndMeterWidth, 225 - y - padding);
    auto controlGroupArea = controlGroup.getLocalBounds().reduced(padding);
    int knobX = controlGroupArea.getX();
    int knobY = controlGroupArea.getY();

    lowCutKnob.setTopLeftPosition(knobX, knobY);
    compressionKnob.setTopLeftPosition(lowCutKnob.getRight() + padding, knobY);
    controlKnob.setTopLeftPosition(compressionKnob.getRight() + padding, knobY);

    outputGroup.setBounds(controlGroup.getRight() + padding, y, outputWidth, bounds.getHeight() - y - padding);
    outputGainKnob.setTopLeftPosition(knobX, knobY);
    outputMeter.setBounds(knobX, outputGainKnob.getBottom() + 42, meterWidth, meterHeight);

    meterGroup.setBounds(padding, controlGroup.getBottom() + padding, controlAndMeterWidth, 225 - padding);
    inputMeter.setBounds(
        knobWidth - (padding * 2),
        controlGroup.getBottom() - meterHeight - 25, 
        meterWidth, 
        meterHeight);
    gRMeter.setBounds(
        inputMeter.getRight() + knobWidth,         // X: directly to right of inputMeter
        meterGroup.getBottom() - meterHeight - 25,  // Y: same as inputMeter
        meterWidth,
        meterHeight);
}

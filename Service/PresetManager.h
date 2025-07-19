/*
  ==============================================================================

    PresetManager.h
    Created: 19 Jul 2025 10:06:45am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace Service
{
    class PresetManager : public juce::ValueTree::Listener
    {
    public:
        static const juce::File defaultDirectory;
        static const juce::String extension;
        static const juce::String presetNameProperty;

        PresetManager(juce::AudioProcessorValueTreeState& apvts);

        void savePreset(const juce::String& presetName);
        void deletePreset(const juce::String& presetName);
        void loadPreset(const juce::String& presetName);

        int loadNextPreset();
        int loadPreviousPreset();

        juce::StringArray getAllPresets() const;
        juce::String getCurrentPreset() const;

    private:
        void valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged) override;

        juce::AudioProcessorValueTreeState& apvts;
        juce::Value currentPreset;
    };
}
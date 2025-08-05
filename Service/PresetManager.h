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
    /**
     * @class PresetManager
     * @brief Handles saving, loading, and deleting audio processor presets.
     * The PresetManager is responsible for creating and managing preset files
     * for the plugin. Presets are stored in a default directory based on the
     * company and project name, and are serialized as XML files using
     * `AudioProcessorValueTreeState`.
     */
    class PresetManager : public juce::ValueTree::Listener
    {
    public:
        /** The default directory for storing presets. */
        static const juce::File defaultDirectory;

        /** The file extension used for preset files. */
        static const juce::String extension;

        /** The ValueTree property name used to store the current preset name. */
        static const juce::String presetNameProperty;

        /**
         * @brief Constructs a PresetManager for a given AudioProcessorValueTreeState.
         * @param apvts The AudioProcessorValueTreeState used to store and manage plugin parameters.
         */
        PresetManager(juce::AudioProcessorValueTreeState& apvts);

        /**
         * @brief Saves the current plugin state as a preset file.
         * @param presetName Name of the preset to save.
         */
        void savePreset(const juce::String& presetName);

        /**
         * @brief Deletes a preset file from the default directory.
         * @param presetName Name of the preset to delete.
         */
        void deletePreset(const juce::String& presetName);

        /**
         * @brief Loads a preset file into the plugin.
         * @param presetName Name of the preset to load.
         */
        void loadPreset(const juce::String& presetName);

        /**
         * @brief Loads the next preset in the list of available presets.
         * @return The index of the loaded preset, or -1 if no presets are available.
         */
        int loadNextPreset();

        /**
         * @brief Loads the previous preset in the list of available presets.
         * @return The index of the loaded preset, or -1 if no presets are available.
         */
        int loadPreviousPreset();

        /**
         * @brief Gets a list of all available preset names.
         * @return A StringArray containing the names of all saved presets.
         */
        juce::StringArray getAllPresets() const;

        /**
         * @brief Gets the name of the currently loaded preset.
         * @return The name of the current preset.
         */
        juce::String getCurrentPreset() const;

    private:
        /** @brief Called when the underlying ValueTree is redirected. */
        void valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged) override;

        juce::AudioProcessorValueTreeState& apvts; /**< Reference to the plugin's APVTS. */
        juce::Value currentPreset; /**< Holds the current preset name as a JUCE Value. */
    };
}

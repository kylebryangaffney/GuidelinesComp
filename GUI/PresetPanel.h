/*
  ==============================================================================

    PresetPanel.h
    Created: 19 Jul 2025 10:23:21am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Service/PresetManager.h"

namespace Gui
{
    class PresetPanel : public juce::Component,
        private juce::Button::Listener,
        private juce::ComboBox::Listener
    {
    public:
        PresetPanel(Service::PresetManager& pm);
        ~PresetPanel() override;

        void resized() override;
        void loadPresetList();

    private:
        void buttonClicked(juce::Button* button) override;
        void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
        void configureButton(juce::Button& button, const juce::String& buttonText);

        juce::TextButton saveButton, deleteButton, previousPresetButton, nextPresetButton;
        juce::ComboBox presetList;

        Service::PresetManager& presetManager;
        std::unique_ptr<juce::FileChooser> fileChooser;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetPanel)
    };
}


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
#include "CheckmarkButton.h"
#include "XButton.h"

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

        std::unique_ptr<CheckmarkButton> saveButton;
        std::unique_ptr<XButton> deleteButton;
        juce::TextButton previousPresetButton, nextPresetButton;
        juce::ComboBox presetList;

        Service::PresetManager& presetManager;
        std::unique_ptr<juce::FileChooser> fileChooser;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetPanel)
    };
}


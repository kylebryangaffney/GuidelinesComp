
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
        // Listener overrides
        void buttonClicked(juce::Button* button) override;
        void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

        // Action menu handling
        void handleActionMenuResult(int result);
        static void popupMenuCallback(int result,
            juce::Component::SafePointer<PresetPanel> safePanel);

        // File chooser handling
        void handleFileChooserResult(const juce::FileChooser& chooser);
        static void fileChooserCallback(const juce::FileChooser& chooser,
            juce::Component::SafePointer<PresetPanel> safePanel);

        // Internal helpers
        void configureButton(juce::Button& button, const juce::String& buttonText);

        // --- Members ---
        Service::PresetManager& presetManager;

        std::unique_ptr<juce::DrawableButton> actionButton;
        juce::TextButton previousPresetButton, nextPresetButton;
        juce::ComboBox presetList;

        std::unique_ptr<juce::FileChooser> fileChooser;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetPanel)
    };
}

/*
  ==============================================================================

    PresetPanel.cpp
    Created: 19 Jul 2025 10:26:38am
    Author:  kyleb

  ==============================================================================
*/

#include "PresetPanel.h"

namespace Gui
{

    PresetPanel::PresetPanel(Service::PresetManager& pm)
        : presetManager(pm)
    {
        configureButton(saveButton, "Save");
        configureButton(deleteButton, "Delete");
        configureButton(previousPresetButton, "<");
        configureButton(nextPresetButton, ">");

        presetList.setTextWhenNothingSelected("No Preset Selected");
        presetList.setMouseCursor(juce::MouseCursor::PointingHandCursor);
        addAndMakeVisible(presetList);
        presetList.addListener(this);

        loadPresetList();
    }

    PresetPanel::~PresetPanel()
    {
        saveButton.removeListener(this);
        deleteButton.removeListener(this);
        previousPresetButton.removeListener(this);
        nextPresetButton.removeListener(this);
        presetList.removeListener(this);
    }

    void PresetPanel::resized()
    {
        const auto container = getLocalBounds().reduced(4);
        auto bounds = container;

        saveButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.2f)).reduced(4));
        previousPresetButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(4));
        presetList.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.4f)).reduced(4));
        nextPresetButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(4));
        deleteButton.setBounds(bounds.reduced(4));
    }

    void PresetPanel::loadPresetList()
    {
        presetList.clear(juce::dontSendNotification);

        const auto allPresets = presetManager.getAllPresets();
        const auto currentPreset = presetManager.getCurrentPreset();

        presetList.addItemList(allPresets, 1);
        presetList.setSelectedItemIndex(allPresets.indexOf(currentPreset), false);
    }

    void PresetPanel::buttonClicked(juce::Button* button)
    {
        if (button == &saveButton)
        {
            fileChooser = std::make_unique<juce::FileChooser>(
                "Please enter the name of the preset to save",
                Service::PresetManager::defaultDirectory,
                "*." + Service::PresetManager::extension
            );

            fileChooser->launchAsync(juce::FileBrowserComponent::saveMode, [&](const juce::FileChooser& chooser)
                {
                    const auto resultFile = chooser.getResult();
                    presetManager.savePreset(resultFile.getFileNameWithoutExtension());
                    loadPresetList();
                });
        }
        else if (button == &previousPresetButton)
        {
            const int index = presetManager.loadPreviousPreset();
            presetList.setSelectedItemIndex(index, false);
        }
        else if (button == &nextPresetButton)
        {
            const int index = presetManager.loadNextPreset();
            presetList.setSelectedItemIndex(index, false);
        }
        else if (button == &deleteButton)
        {
            presetManager.deletePreset(presetManager.getCurrentPreset());
            loadPresetList();
        }
    }

    void PresetPanel::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
    {
        if (comboBoxThatHasChanged == &presetList)
        {
            presetManager.loadPreset(presetList.getItemText(presetList.getSelectedItemIndex()));
        }
    }

    void PresetPanel::configureButton(juce::Button& button, const juce::String& buttonText)
    {
        button.setButtonText(buttonText);
        button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
        addAndMakeVisible(button);
        button.addListener(this);
    }

} // namespace Gui

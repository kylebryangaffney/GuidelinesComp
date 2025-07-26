/*
  ==============================================================================

    PresetPanel.cpp
    Created: 19 Jul 2025 10:26:38am
    Author:  kyleb

  ==============================================================================
*/

#include "PresetPanel.h"
#include "LookAndFeel.h"

namespace Gui
{

    PresetPanel::PresetPanel(Service::PresetManager& pm)
        : presetManager(pm)
    {

        saveButton = std::make_unique<CheckmarkButton>();
        addAndMakeVisible(*saveButton);
        saveButton->addListener(this);

        deleteButton = std::make_unique<XButton>();
        addAndMakeVisible(*deleteButton);
        deleteButton->addListener(this);

        configureButton(previousPresetButton, u8"\u25C0");
        configureButton(nextPresetButton, u8"\u25B6");

        presetList.setTextWhenNothingSelected("Select Preset");
        presetList.setMouseCursor(juce::MouseCursor::PointingHandCursor);
        addAndMakeVisible(presetList);
        presetList.addListener(this);

        loadPresetList();
        setLookAndFeel(PresetPanelLookAndFeel::get());
    }

    PresetPanel::~PresetPanel()
    {
        saveButton->removeListener(this);
        deleteButton->removeListener(this);
        previousPresetButton.removeListener(this);
        nextPresetButton.removeListener(this);
        presetList.removeListener(this);
    }
    void PresetPanel::resized()
    {
        const int reduce = 4;
        juce::Rectangle<int> area = getLocalBounds().reduced(reduce);

        // Split into two rows: button row and list row
        const int buttonRowHeight = area.getHeight() / 2;
        juce::Rectangle<int> buttonRow = area.removeFromTop(buttonRowHeight);
        juce::Rectangle<int> listRow = area;

        // Divide button row into 4 equal parts
        const int buttonWidth = buttonRow.getWidth() / 4;

        juce::Rectangle<int> prevBounds = buttonRow.removeFromLeft(buttonWidth).reduced(reduce);
        juce::Rectangle<int> nextBounds = buttonRow.removeFromLeft(buttonWidth).reduced(reduce);
        juce::Rectangle<int> saveBounds = buttonRow.removeFromLeft(buttonWidth).reduced(reduce);
        juce::Rectangle<int> deleteBounds = buttonRow.reduced(reduce);

        // Assign bounds
        previousPresetButton.setBounds(prevBounds);
        nextPresetButton.setBounds(nextBounds);
        saveButton->setBounds(saveBounds);
        deleteButton->setBounds(deleteBounds);
        presetList.setBounds(listRow.reduced(reduce));
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
        if (button == saveButton.get())
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
        else if (button == deleteButton.get())
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

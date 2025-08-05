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

    //==============================================================================
    PresetPanel::PresetPanel(Service::PresetManager& pm)
        : presetManager(pm)
    {
        // Action button with gear drawable
        actionButton = std::make_unique<juce::DrawableButton>(
            "ActionButton", juce::DrawableButton::ImageOnButtonBackground);

        std::unique_ptr<juce::Drawable> defaultDrawable = juce::Drawable::createFromImageData(
            BinaryData::gearDefault_svg, BinaryData::gearDefault_svgSize);
        std::unique_ptr<juce::Drawable> hoverDrawable = juce::Drawable::createFromImageData(
            BinaryData::gearHover_svg, BinaryData::gearHover_svgSize);
        std::unique_ptr<juce::Drawable> pressedDrawable = juce::Drawable::createFromImageData(
            BinaryData::gearPressed_svg, BinaryData::gearPressed_svgSize);

        actionButton->setImages(defaultDrawable.get(), hoverDrawable.get(), pressedDrawable.get(),
            nullptr, nullptr);

        addAndMakeVisible(*actionButton);
        actionButton->addListener(this);

        // Preset list
        presetList.setTextWhenNothingSelected("Select Preset");
        presetList.setMouseCursor(juce::MouseCursor::PointingHandCursor);
        addAndMakeVisible(presetList);
        presetList.addListener(this);

        loadPresetList();
        setLookAndFeel(PresetPanelLookAndFeel::get());
    }

    PresetPanel::~PresetPanel()
    {
        actionButton->removeListener(this);
        previousPresetButton.removeListener(this);
        nextPresetButton.removeListener(this);
        presetList.removeListener(this);
    }

    //==============================================================================
    void PresetPanel::resized()
    {
        const int reduce = 4;
        auto area = getLocalBounds().reduced(reduce);

        const int buttonRowHeight = area.getHeight() / 2;
        auto buttonRow = area.removeFromTop(buttonRowHeight);
        auto listRow = area;

        const int buttonWidth = buttonRow.getWidth() / 4;

        auto prevBounds = buttonRow.removeFromLeft(buttonWidth).reduced(reduce);
        auto nextBounds = buttonRow.removeFromLeft(buttonWidth).reduced(reduce);
        auto actionBounds = buttonRow.removeFromLeft(buttonWidth).reduced(reduce);

        previousPresetButton.setBounds(prevBounds);
        nextPresetButton.setBounds(nextBounds);
        actionButton->setBounds(actionBounds);
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

    //==============================================================================
    // Event Handlers

    void PresetPanel::buttonClicked(juce::Button* button)
    {
        if (button == actionButton.get())
        {
            juce::PopupMenu menu;
            menu.addItem(1, "Save New");
            menu.addItem(2, "Overwrite Current");
            menu.addItem(3, "Delete");

            auto safeThis = juce::Component::SafePointer<PresetPanel>(this);

            menu.showMenuAsync(
                juce::PopupMenu::Options().withTargetComponent(*actionButton),
                [safeThis](int result)
                {
                    PresetPanel::popupMenuCallback(result, safeThis);
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
    }

    void PresetPanel::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
    {
        if (comboBoxThatHasChanged == &presetList)
        {
            presetManager.loadPreset(presetList.getItemText(presetList.getSelectedItemIndex()));
        }
    }

    //==============================================================================
    // Action Menu

    void PresetPanel::handleActionMenuResult(int result)
    {
        switch (result)
        {
        case 1: // Save New
        {
            fileChooser = std::make_unique<juce::FileChooser>(
                "Enter preset name to save",
                Service::PresetManager::defaultDirectory,
                "*." + Service::PresetManager::extension);

            auto safeThis = juce::Component::SafePointer<PresetPanel>(this);

            fileChooser->launchAsync(
                juce::FileBrowserComponent::saveMode,
                [safeThis](const juce::FileChooser& chooser)
                {
                    PresetPanel::fileChooserCallback(chooser, safeThis);
                });
            break;
        }
        case 2: // Overwrite
        {
            presetManager.savePreset(presetManager.getCurrentPreset());
            loadPresetList();
            break;
        }
        case 3: // Delete
        {
            presetManager.deletePreset(presetManager.getCurrentPreset());
            loadPresetList();
            break;
        }
        default:
            break;
        }
    }

    void PresetPanel::popupMenuCallback(int result,
        juce::Component::SafePointer<PresetPanel> safePanel)
    {
        if (safePanel != nullptr)
            safePanel->handleActionMenuResult(result);
    }

    //==============================================================================
    // File Chooser

    void PresetPanel::fileChooserCallback(const juce::FileChooser& chooser,
        juce::Component::SafePointer<PresetPanel> safePanel)
    {
        if (safePanel != nullptr)
            safePanel->handleFileChooserResult(chooser);
    }

    void PresetPanel::handleFileChooserResult(const juce::FileChooser& chooser)
    {
        auto resultFile = chooser.getResult();
        if (resultFile.existsAsFile() || resultFile.getParentDirectory().isDirectory())
        {
            presetManager.savePreset(resultFile.getFileNameWithoutExtension());
            loadPresetList();
        }
    }

    //==============================================================================
    // Helpers

    void PresetPanel::configureButton(juce::Button& button, const juce::String& buttonText)
    {
        button.setButtonText(buttonText);
        button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
        addAndMakeVisible(button);
        button.addListener(this);
    }

} // namespace Gui


/*
  ==============================================================================

    PresetManager.cpp
    Created: 19 Jul 2025 10:06:45am
    Author:  kyleb

  ==============================================================================
*/

#include "../Service/PresetManager.h"

namespace Service
{
    const juce::File PresetManager::defaultDirectory{
        juce::File::getSpecialLocation(juce::File::SpecialLocationType::commonDocumentsDirectory)
            .getChildFile(ProjectInfo::companyName)
            .getChildFile(ProjectInfo::projectName)
    };

    const juce::String PresetManager::extension{ "preset" };
    const juce::String PresetManager::presetNameProperty{ "presetName" };

    //==============================================================================
    PresetManager::PresetManager(juce::AudioProcessorValueTreeState& apvts)
        : apvts(apvts)
    {
        if (!defaultDirectory.exists())
        {
            const auto result = defaultDirectory.createDirectory();
            if (result.failed())
            {
                DBG("Could not create preset directory: " + result.getErrorMessage());
                jassertfalse;
            }
        }

        apvts.state.addListener(this);
        currentPreset.referTo(apvts.state.getPropertyAsValue(presetNameProperty, nullptr));
    }

    //==============================================================================
    void PresetManager::savePreset(const juce::String& presetName)
    {
        if (presetName.isEmpty())
            return;

        currentPreset.setValue(presetName);

        auto xml = apvts.copyState().createXml();
        auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);

        if (!xml->writeTo(presetFile))
        {
            DBG("Could not create this preset file");
            jassertfalse;
        }
    }

    void PresetManager::deletePreset(const juce::String& presetName)
    {
        if (presetName.isEmpty())
            return;

        auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);

        if (!presetFile.existsAsFile())
        {
            DBG("Preset File: " + presetFile.getFullPathName() + " does not exist");
            jassertfalse;
            return;
        }

        if (!presetFile.deleteFile())
        {
            DBG("Could not delete Preset File: " + presetFile.getFullPathName());
            jassertfalse;
            return;
        }

        currentPreset.setValue("");
    }

    void PresetManager::loadPreset(const juce::String& presetName)
    {
        if (presetName.isEmpty())
            return;

        auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);

        if (!presetFile.existsAsFile())
        {
            DBG("Preset File: " + presetFile.getFullPathName() + " does not exist");
            jassertfalse;
            return;
        }

        juce::XmlDocument xmlDocument{ presetFile };
        auto xmlElement = xmlDocument.getDocumentElement();

        if (xmlElement == nullptr)
        {
            DBG("Failed to load XML from file: " + presetFile.getFullPathName());
            jassertfalse;
            return;
        }

        juce::ValueTree newState = juce::ValueTree::fromXml(*xmlElement);
        apvts.replaceState(newState);
        currentPreset.setValue(presetName);
    }

    //==============================================================================
    int PresetManager::loadNextPreset()
    {
        auto allPresets = getAllPresets();
        if (allPresets.isEmpty())
            return -1;

        int currentIndex = allPresets.indexOf(currentPreset.toString());
        int nextIndex = (currentIndex + 1 > allPresets.size() - 1) ? 0 : currentIndex + 1;

        loadPreset(allPresets.getReference(nextIndex));
        return nextIndex;
    }

    int PresetManager::loadPreviousPreset()
    {
        auto allPresets = getAllPresets();
        if (allPresets.isEmpty())
            return -1;

        int currentIndex = allPresets.indexOf(currentPreset.toString());
        int previousIndex = (currentIndex - 1 < 0) ? allPresets.size() - 1 : currentIndex - 1;

        loadPreset(allPresets.getReference(previousIndex));
        return previousIndex;
    }

    //==============================================================================
    juce::StringArray PresetManager::getAllPresets() const
    {
        juce::StringArray presetList;

        auto fileArray = defaultDirectory.findChildFiles(
            juce::File::TypesOfFileToFind::findFiles,
            false,
            "*." + extension
        );

        for (const auto& file : fileArray)
            presetList.add(file.getFileNameWithoutExtension());

        return presetList;
    }

    juce::String PresetManager::getCurrentPreset() const
    {
        return currentPreset.toString();
    }

    //==============================================================================
    void PresetManager::valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged)
    {
        currentPreset.referTo(treeWhichHasBeenChanged.getPropertyAsValue(presetNameProperty, nullptr));
    }
} // namespace Service
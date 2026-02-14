#include "PresetComponent.h"
#include "CustomLookAndFeel.h"

PresetComponent::PresetComponent(PresetManager& pm)
    : presetManager(pm),
      safeFlag(std::make_shared<bool>(true)) {

    presetLabel.setText("Preset:", juce::dontSendNotification);
    presetLabel.setJustificationType(juce::Justification::centredRight);
    presetLabel.setColour(juce::Label::textColourId, CustomLookAndFeel::textColorDim);
    addAndMakeVisible(presetLabel);

    presetSelector.onChange = [this] { loadSelectedPreset(); };
    addAndMakeVisible(presetSelector);

    prevButton.setButtonText("<");
    prevButton.onClick = [this] { navigatePreset(-1); };
    addAndMakeVisible(prevButton);

    nextButton.setButtonText(">");
    nextButton.onClick = [this] { navigatePreset(1); };
    addAndMakeVisible(nextButton);

    saveButton.setButtonText("Save");
    saveButton.onClick = [this] { saveCurrentPreset(); };
    addAndMakeVisible(saveButton);

    loadButton.setButtonText("Load");
    loadButton.onClick = [this] { loadFromFile(); };
    addAndMakeVisible(loadButton);

    refreshPresetList();
}

PresetComponent::~PresetComponent() {
    *safeFlag = false;
}

void PresetComponent::refreshPresetList() {
    presetSelector.clear();

    auto names = presetManager.getPresetNames();
    int id = 1;
    for (const auto& name : names) {
        presetSelector.addItem(name, id++);
    }

    if (presetSelector.getNumItems() > 0) {
        presetSelector.setSelectedId(1, juce::dontSendNotification);
    }
}

void PresetComponent::paint(juce::Graphics& g) {
    // No background - blends into header
}

void PresetComponent::resized() {
    auto bounds = getLocalBounds().reduced(2, 0);

    presetLabel.setBounds(bounds.removeFromLeft(46));
    bounds.removeFromLeft(4);

    prevButton.setBounds(bounds.removeFromLeft(24));
    bounds.removeFromLeft(2);

    nextButton.setBounds(bounds.removeFromLeft(24));
    bounds.removeFromLeft(6);

    loadButton.setBounds(bounds.removeFromRight(44));
    bounds.removeFromRight(4);

    saveButton.setBounds(bounds.removeFromRight(44));
    bounds.removeFromRight(6);

    presetSelector.setBounds(bounds);
}

void PresetComponent::loadSelectedPreset() {
    auto name = presetSelector.getText();
    SynthPreset preset;

    if (presetManager.loadPresetByName(name, preset)) {
        presetManager.setCurrentPreset(preset);
        if (onPresetLoaded) {
            onPresetLoaded(preset);
        }
    }
}

void PresetComponent::saveCurrentPreset() {
    if (!onGetCurrentPreset)
        return;

    auto preset = onGetCurrentPreset();

    auto flag = safeFlag;

    auto fileChooser = std::make_shared<juce::FileChooser>(
        "Save Preset",
        presetManager.getPresetsDirectory(),
        "*.csspresets"
    );

    fileChooser->launchAsync(
        juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
        [this, fileChooser, preset, flag](const juce::FileChooser& fc) {
            if (!*flag)
                return;

            auto file = fc.getResult();
            if (file != juce::File{}) {
                auto presetToSave = preset;
                presetToSave.name = file.getFileNameWithoutExtension();

                auto fileWithExt = file.withFileExtension(".csspresets");
                if (presetManager.savePreset(presetToSave, fileWithExt)) {
                    refreshPresetList();
                }
            }
        }
    );
}

void PresetComponent::loadFromFile() {
    auto flag = safeFlag;

    auto fileChooser = std::make_shared<juce::FileChooser>(
        "Load Preset",
        presetManager.getPresetsDirectory(),
        "*.csspresets"
    );

    fileChooser->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, fileChooser, flag](const juce::FileChooser& fc) {
            if (!*flag)
                return;

            auto file = fc.getResult();
            if (file.existsAsFile()) {
                SynthPreset preset;
                if (presetManager.loadPreset(preset, file)) {
                    presetManager.setCurrentPreset(preset);
                    refreshPresetList();
                    if (onPresetLoaded) {
                        onPresetLoaded(preset);
                    }
                }
            }
        }
    );
}

void PresetComponent::navigatePreset(int direction) {
    int currentIndex = presetSelector.getSelectedItemIndex();
    int newIndex = currentIndex + direction;

    if (newIndex >= 0 && newIndex < presetSelector.getNumItems()) {
        presetSelector.setSelectedItemIndex(newIndex);
    }
}

#pragma once
#include <JuceHeader.h>
#include "../Preset/PresetManager.h"

class PresetComponent : public juce::Component {
public:
    PresetComponent(PresetManager& pm);
    ~PresetComponent() override;

    void resized() override;
    void paint(juce::Graphics& g) override;
    
    void refreshPresetList();

    // Callbacks
    std::function<void(const SynthPreset&)> onPresetLoaded;
    std::function<SynthPreset()> onGetCurrentPreset;

private:
    PresetManager& presetManager;
    
    juce::ComboBox presetSelector;
    juce::TextButton saveButton;
    juce::TextButton loadButton;
    juce::TextButton prevButton;
    juce::TextButton nextButton;
    
    juce::Label presetLabel;
    
    // Flag to track if component is being destroyed
    std::shared_ptr<bool> safeFlag;

    void loadSelectedPreset();
    void saveCurrentPreset();
    void loadFromFile();
    void navigatePreset(int direction);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetComponent)
};

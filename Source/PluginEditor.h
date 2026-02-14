#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/CustomLookAndFeel.h"
#include "GUI/OscillatorComponent.h"
#include "GUI/AdsrComponent.h"
#include "GUI/LfoComponent.h"
#include "GUI/FilterComponent.h"
#include "GUI/EffectsComponent.h"
#include "GUI/PresetComponent.h"
#include "Preset/PresetManager.h"

class CodingSoundsSynthEditor : public juce::AudioProcessorEditor {
public:
    explicit CodingSoundsSynthEditor(CodingSoundsSynthProcessor&);
    ~CodingSoundsSynthEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void updateSynthParameters();
    void applyPreset(const SynthPreset& preset);
    SynthPreset getCurrentPreset();

    CodingSoundsSynthProcessor& processorRef;
    
    CustomLookAndFeel customLookAndFeel;
    
    juce::Label titleLabel;
    
    PresetManager presetManager;
    std::unique_ptr<PresetComponent> presetComponent;
    
    std::unique_ptr<OscillatorComponent> osc1Component;
    std::unique_ptr<OscillatorComponent> osc2Component;
    std::unique_ptr<FilterComponent> filterComponent;
    std::unique_ptr<AdsrComponent> ampAdsrComponent;
    std::unique_ptr<AdsrComponent> filterAdsrComponent;
    std::unique_ptr<LfoComponent> lfoComponent;
    std::unique_ptr<EffectsComponent> effectsComponent;
    
    juce::Slider masterGainSlider;
    juce::Label masterGainLabel;
    
    juce::MidiKeyboardComponent keyboardComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CodingSoundsSynthEditor)
};

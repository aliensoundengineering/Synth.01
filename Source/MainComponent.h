#pragma once
#include <JuceHeader.h>
#include "SynthAudioSource.h"
#include "Preset/PresetManager.h"
#include "GUI/CustomLookAndFeel.h"
#include "GUI/OscillatorComponent.h"
#include "GUI/AdsrComponent.h"
#include "GUI/LfoComponent.h"
#include "GUI/FilterComponent.h"
#include "GUI/EffectsComponent.h"
#include "GUI/PresetComponent.h"

class MainComponent : public juce::Component {
public:
    MainComponent();
    ~MainComponent() override;

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    void updateSynthParameters();
    void applyPreset(const SynthPreset& preset);
    SynthPreset getCurrentPreset();

    // Audio
    juce::AudioDeviceManager deviceManager;
    juce::MidiKeyboardState keyboardState;
    SynthAudioSource synthSource;
    juce::AudioSourcePlayer audioSourcePlayer;
    
    // Preset management
    PresetManager presetManager;
    
    // Custom look and feel
    CustomLookAndFeel customLookAndFeel;
    
    // GUI Components
    std::unique_ptr<PresetComponent> presetComponent;
    std::unique_ptr<OscillatorComponent> osc1Component;
    std::unique_ptr<OscillatorComponent> osc2Component;
    std::unique_ptr<FilterComponent> filterComponent;
    std::unique_ptr<AdsrComponent> ampAdsrComponent;
    std::unique_ptr<AdsrComponent> filterAdsrComponent;
    std::unique_ptr<LfoComponent> lfoComponent;
    std::unique_ptr<EffectsComponent> effectsComponent;
    
    // Master controls
    juce::Slider masterGainSlider;
    juce::Label masterGainLabel;
    
    // Keyboard
    juce::MidiKeyboardComponent keyboardComponent;
    
    // Title
    juce::Label titleLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

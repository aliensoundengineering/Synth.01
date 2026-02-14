#pragma once
#include <JuceHeader.h>
#include "../Synth/SynthVoice.h"
#include "../Effects/DriveDistortion.h"
#include "../Effects/DelayLine.h"

// Complete preset structure
struct SynthPreset {
    juce::String name = "Init";
    SynthParameters synthParams;
    
    // Drive parameters
    DriveType driveType = DriveType::Soft;
    float driveAmount = 0.0f;
    float driveMix = 1.0f;
    float driveOutputGain = 0.7f;
    
    // Delay parameters
    float delayTime = 250.0f;
    float delayFeedback = 0.4f;
    float delayMix = 0.3f;

    // Reverb parameters
    float reverbRoomSize = 0.5f;
    float reverbDamping = 0.5f;
    float reverbWetLevel = 0.3f;
    float reverbDryLevel = 0.7f;
    float reverbWidth = 1.0f;
};

class PresetManager {
public:
    PresetManager();
    ~PresetManager() = default;

    // File operations
    bool savePreset(const SynthPreset& preset, const juce::File& file);
    bool loadPreset(SynthPreset& preset, const juce::File& file);
    
    // Factory presets
    void initFactoryPresets();
    const std::vector<SynthPreset>& getFactoryPresets() const { return factoryPresets; }
    
    // User presets directory
    juce::File getPresetsDirectory() const;
    juce::Array<juce::File> getUserPresetFiles() const;
    
    // Current preset management
    void setCurrentPreset(const SynthPreset& preset);
    const SynthPreset& getCurrentPreset() const { return currentPreset; }
    
    // Preset list
    juce::StringArray getPresetNames() const;
    bool loadPresetByName(const juce::String& name, SynthPreset& preset);

private:
    juce::var presetToVar(const SynthPreset& preset);
    bool varToPreset(const juce::var& var, SynthPreset& preset);
    
    static juce::String waveTypeToString(WaveType type);
    static WaveType stringToWaveType(const juce::String& str);
    static juce::String filterTypeToString(FilterType type);
    static FilterType stringToFilterType(const juce::String& str);
    static juce::String lfoWaveToString(LfoWaveType type);
    static LfoWaveType stringToLfoWave(const juce::String& str);
    static juce::String lfoDestToString(LfoDestination dest);
    static LfoDestination stringToLfoDest(const juce::String& str);
    static juce::String driveTypeToString(DriveType type);
    static DriveType stringToDriveType(const juce::String& str);
    
    std::vector<SynthPreset> factoryPresets;
    SynthPreset currentPreset;
};

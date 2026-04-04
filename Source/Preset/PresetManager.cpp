#include "PresetManager.h"

PresetManager::PresetManager() {
    initFactoryPresets();
    if (!factoryPresets.empty()) {
        currentPreset = factoryPresets[0];
    }
}

juce::File PresetManager::getPresetsDirectory() const {
    auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    auto presetsDir = appDataDir.getChildFile("CodingSoundsSynth").getChildFile("Presets");
    
    if (!presetsDir.exists()) {
        presetsDir.createDirectory();
    }
    
    return presetsDir;
}

juce::Array<juce::File> PresetManager::getUserPresetFiles() const {
    auto presetsDir = getPresetsDirectory();
    return presetsDir.findChildFiles(juce::File::findFiles, false, "*.csspresets");
}

juce::String PresetManager::waveTypeToString(WaveType type) {
    switch (type) {
        case WaveType::Sine: return "Sine";
        case WaveType::Saw: return "Saw";
        case WaveType::Square: return "Square";
    }
    return "Saw";
}

WaveType PresetManager::stringToWaveType(const juce::String& str) {
    if (str == "Sine") return WaveType::Sine;
    if (str == "Square") return WaveType::Square;
    return WaveType::Saw;
}

juce::String PresetManager::filterTypeToString(FilterType type) {
    switch (type) {
        case FilterType::LowPass: return "LowPass";
        case FilterType::HighPass: return "HighPass";
        case FilterType::BandPass: return "BandPass";
    }
    return "LowPass";
}

FilterType PresetManager::stringToFilterType(const juce::String& str) {
    if (str == "HighPass") return FilterType::HighPass;
    if (str == "BandPass") return FilterType::BandPass;
    return FilterType::LowPass;
}

juce::String PresetManager::lfoWaveToString(LfoWaveType type) {
    switch (type) {
        case LfoWaveType::Sine: return "Sine";
        case LfoWaveType::Triangle: return "Triangle";
        case LfoWaveType::Square: return "Square";
        case LfoWaveType::SawUp: return "SawUp";
        case LfoWaveType::SawDown: return "SawDown";
        case LfoWaveType::Random: return "Random";
    }
    return "Sine";
}

LfoWaveType PresetManager::stringToLfoWave(const juce::String& str) {
    if (str == "Triangle") return LfoWaveType::Triangle;
    if (str == "Square") return LfoWaveType::Square;
    if (str == "SawUp") return LfoWaveType::SawUp;
    if (str == "SawDown") return LfoWaveType::SawDown;
    if (str == "Random") return LfoWaveType::Random;
    return LfoWaveType::Sine;
}

juce::String PresetManager::lfoDestToString(LfoDestination dest) {
    switch (dest) {
        case LfoDestination::None: return "None";
        case LfoDestination::FilterCutoff: return "FilterCutoff";
        case LfoDestination::Osc1Pitch: return "Osc1Pitch";
        case LfoDestination::Osc2Pitch: return "Osc2Pitch";
        case LfoDestination::Osc1Gain: return "Osc1Gain";
        case LfoDestination::Osc2Gain: return "Osc2Gain";
    }
    return "FilterCutoff";
}

LfoDestination PresetManager::stringToLfoDest(const juce::String& str) {
    if (str == "None") return LfoDestination::None;
    if (str == "Osc1Pitch") return LfoDestination::Osc1Pitch;
    if (str == "Osc2Pitch") return LfoDestination::Osc2Pitch;
    if (str == "Osc1Gain") return LfoDestination::Osc1Gain;
    if (str == "Osc2Gain") return LfoDestination::Osc2Gain;
    return LfoDestination::FilterCutoff;
}

juce::String PresetManager::driveTypeToString(DriveType type) {
    switch (type) {
        case DriveType::Soft: return "Soft";
        case DriveType::Hard: return "Hard";
        case DriveType::Tube: return "Tube";
        case DriveType::Fuzz: return "Fuzz";
    }
    return "Soft";
}

DriveType PresetManager::stringToDriveType(const juce::String& str) {
    if (str == "Hard") return DriveType::Hard;
    if (str == "Tube") return DriveType::Tube;
    if (str == "Fuzz") return DriveType::Fuzz;
    return DriveType::Soft;
}

juce::var PresetManager::presetToVar(const SynthPreset& preset) {
    auto obj = new juce::DynamicObject();
    
    obj->setProperty("name", preset.name);
    
    // Oscillator 1
    obj->setProperty("osc1Wave", waveTypeToString(preset.synthParams.osc1Wave));
    obj->setProperty("osc1Gain", preset.synthParams.osc1Gain);
    obj->setProperty("osc1Detune", preset.synthParams.osc1Detune);
    obj->setProperty("osc1Octave", preset.synthParams.osc1Octave);
    
    // Oscillator 2
    obj->setProperty("osc2Wave", waveTypeToString(preset.synthParams.osc2Wave));
    obj->setProperty("osc2Gain", preset.synthParams.osc2Gain);
    obj->setProperty("osc2Detune", preset.synthParams.osc2Detune);
    obj->setProperty("osc2Octave", preset.synthParams.osc2Octave);
    
    // Filter
    obj->setProperty("filterType", filterTypeToString(preset.synthParams.filterType));
    obj->setProperty("filterCutoff", preset.synthParams.filterCutoff);
    obj->setProperty("filterResonance", preset.synthParams.filterResonance);
    obj->setProperty("filterEnvAmount", preset.synthParams.filterEnvAmount);
    
    // Amp ADSR
    obj->setProperty("ampAttack", preset.synthParams.ampAdsr.attack);
    obj->setProperty("ampDecay", preset.synthParams.ampAdsr.decay);
    obj->setProperty("ampSustain", preset.synthParams.ampAdsr.sustain);
    obj->setProperty("ampRelease", preset.synthParams.ampAdsr.release);
    
    // Filter ADSR
    obj->setProperty("filterAttack", preset.synthParams.filterAdsr.attack);
    obj->setProperty("filterDecay", preset.synthParams.filterAdsr.decay);
    obj->setProperty("filterSustain", preset.synthParams.filterAdsr.sustain);
    obj->setProperty("filterRelease", preset.synthParams.filterAdsr.release);
    
    // LFO
    obj->setProperty("lfoWave", lfoWaveToString(preset.synthParams.lfoWave));
    obj->setProperty("lfoRate", preset.synthParams.lfoRate);
    obj->setProperty("lfoDepth", preset.synthParams.lfoDepth);
    obj->setProperty("lfoDestination", lfoDestToString(preset.synthParams.lfoDestination));
    
    // Master
    obj->setProperty("masterGain", preset.synthParams.masterGain);
    
    // Drive
    obj->setProperty("driveType", driveTypeToString(preset.driveType));
    obj->setProperty("driveAmount", preset.driveAmount);
    obj->setProperty("driveMix", preset.driveMix);
    obj->setProperty("driveOutputGain", preset.driveOutputGain);
    
    // Delay
    obj->setProperty("delayTime", preset.delayTime);
    obj->setProperty("delayFeedback", preset.delayFeedback);
    obj->setProperty("delayMix", preset.delayMix);

    // Reverb
    obj->setProperty("reverbRoomSize", preset.reverbRoomSize);
    obj->setProperty("reverbDamping", preset.reverbDamping);
    obj->setProperty("reverbWetLevel", preset.reverbWetLevel);
    obj->setProperty("reverbDryLevel", preset.reverbDryLevel);
    obj->setProperty("reverbWidth", preset.reverbWidth);
    
    return juce::var(obj);
}

bool PresetManager::varToPreset(const juce::var& var, SynthPreset& preset) {
    if (!var.isObject())
        return false;
    
    auto* obj = var.getDynamicObject();
    if (!obj)
        return false;
    
    preset.name = obj->getProperty("name").toString();
    
    // Oscillator 1
    preset.synthParams.osc1Wave = stringToWaveType(obj->getProperty("osc1Wave").toString());
    preset.synthParams.osc1Gain = static_cast<float>(obj->getProperty("osc1Gain"));
    preset.synthParams.osc1Detune = static_cast<float>(obj->getProperty("osc1Detune"));
    preset.synthParams.osc1Octave = static_cast<int>(obj->getProperty("osc1Octave"));
    
    // Oscillator 2
    preset.synthParams.osc2Wave = stringToWaveType(obj->getProperty("osc2Wave").toString());
    preset.synthParams.osc2Gain = static_cast<float>(obj->getProperty("osc2Gain"));
    preset.synthParams.osc2Detune = static_cast<float>(obj->getProperty("osc2Detune"));
    preset.synthParams.osc2Octave = static_cast<int>(obj->getProperty("osc2Octave"));
    
    // Filter
    preset.synthParams.filterType = stringToFilterType(obj->getProperty("filterType").toString());
    preset.synthParams.filterCutoff = static_cast<float>(obj->getProperty("filterCutoff"));
    preset.synthParams.filterResonance = static_cast<float>(obj->getProperty("filterResonance"));
    preset.synthParams.filterEnvAmount = static_cast<float>(obj->getProperty("filterEnvAmount"));
    
    // Amp ADSR
    preset.synthParams.ampAdsr.attack = static_cast<float>(obj->getProperty("ampAttack"));
    preset.synthParams.ampAdsr.decay = static_cast<float>(obj->getProperty("ampDecay"));
    preset.synthParams.ampAdsr.sustain = static_cast<float>(obj->getProperty("ampSustain"));
    preset.synthParams.ampAdsr.release = static_cast<float>(obj->getProperty("ampRelease"));
    
    // Filter ADSR
    preset.synthParams.filterAdsr.attack = static_cast<float>(obj->getProperty("filterAttack"));
    preset.synthParams.filterAdsr.decay = static_cast<float>(obj->getProperty("filterDecay"));
    preset.synthParams.filterAdsr.sustain = static_cast<float>(obj->getProperty("filterSustain"));
    preset.synthParams.filterAdsr.release = static_cast<float>(obj->getProperty("filterRelease"));
    
    // LFO
    preset.synthParams.lfoWave = stringToLfoWave(obj->getProperty("lfoWave").toString());
    preset.synthParams.lfoRate = static_cast<float>(obj->getProperty("lfoRate"));
    preset.synthParams.lfoDepth = static_cast<float>(obj->getProperty("lfoDepth"));
    preset.synthParams.lfoDestination = stringToLfoDest(obj->getProperty("lfoDestination").toString());
    
    // Master
    preset.synthParams.masterGain = static_cast<float>(obj->getProperty("masterGain"));
    
    // Drive
    preset.driveType = stringToDriveType(obj->getProperty("driveType").toString());
    preset.driveAmount = static_cast<float>(obj->getProperty("driveAmount"));
    preset.driveMix = static_cast<float>(obj->getProperty("driveMix"));
    preset.driveOutputGain = static_cast<float>(obj->getProperty("driveOutputGain"));
    
    // Delay
    preset.delayTime = static_cast<float>(obj->getProperty("delayTime"));
    preset.delayFeedback = static_cast<float>(obj->getProperty("delayFeedback"));
    preset.delayMix = static_cast<float>(obj->getProperty("delayMix"));

    // Reverb (with defaults for backward compatibility)
    preset.reverbRoomSize = obj->hasProperty("reverbRoomSize") ? static_cast<float>(obj->getProperty("reverbRoomSize")) : 0.5f;
    preset.reverbDamping = obj->hasProperty("reverbDamping") ? static_cast<float>(obj->getProperty("reverbDamping")) : 0.5f;
    preset.reverbWetLevel = obj->hasProperty("reverbWetLevel") ? static_cast<float>(obj->getProperty("reverbWetLevel")) : 0.3f;
    preset.reverbDryLevel = obj->hasProperty("reverbDryLevel") ? static_cast<float>(obj->getProperty("reverbDryLevel")) : 0.7f;
    preset.reverbWidth = obj->hasProperty("reverbWidth") ? static_cast<float>(obj->getProperty("reverbWidth")) : 1.0f;
    
    return true;
}

bool PresetManager::savePreset(const SynthPreset& preset, const juce::File& file) {
    auto json = juce::JSON::toString(presetToVar(preset));
    return file.replaceWithText(json);
}

bool PresetManager::loadPreset(SynthPreset& preset, const juce::File& file) {
    if (!file.existsAsFile())
        return false;
    
    auto content = file.loadFileAsString();
    auto var = juce::JSON::parse(content);
    
    return varToPreset(var, preset);
}

void PresetManager::setCurrentPreset(const SynthPreset& preset) {
    currentPreset = preset;
}

juce::StringArray PresetManager::getPresetNames() const {
    juce::StringArray names;
    
    for (const auto& preset : factoryPresets) {
        names.add(preset.name);
    }
    
    auto userFiles = getUserPresetFiles();
    for (const auto& file : userFiles) {
        names.add(file.getFileNameWithoutExtension());
    }
    
    return names;
}

bool PresetManager::loadPresetByName(const juce::String& name, SynthPreset& preset) {
    // Check factory presets first
    for (const auto& factoryPreset : factoryPresets) {
        if (factoryPreset.name == name) {
            preset = factoryPreset;
            return true;
        }
    }
    
    // Check user presets
    auto file = getPresetsDirectory().getChildFile(name + ".csspresets");
    if (file.existsAsFile()) {
        return loadPreset(preset, file);
    }
    
    return false;
}

void PresetManager::initFactoryPresets() {
    factoryPresets.clear();
    
    // 1. Init - Basic starting point
    {
        SynthPreset init;
        init.name = "Init";
        factoryPresets.push_back(init);
    }
    
    // 2. Fat Bass
    {
        SynthPreset bass;
        bass.name = "Fat Bass";
        bass.synthParams.osc1Wave = WaveType::Saw;
        bass.synthParams.osc1Gain = 0.6f;
        bass.synthParams.osc1Octave = -1;
        bass.synthParams.osc2Wave = WaveType::Square;
        bass.synthParams.osc2Gain = 0.4f;
        bass.synthParams.osc2Octave = -1;
        bass.synthParams.osc2Detune = 10.0f;
        bass.synthParams.filterCutoff = 500.0f;
        bass.synthParams.filterResonance = 2.0f;
        bass.synthParams.filterEnvAmount = 0.6f;
        bass.synthParams.ampAdsr = { 0.01f, 0.1f, 0.9f, 0.1f };
        bass.synthParams.filterAdsr = { 0.01f, 0.3f, 0.2f, 0.1f };
        bass.driveAmount = 0.2f;
        bass.driveType = DriveType::Tube;
        factoryPresets.push_back(bass);
    }
    
    // 3. Bright Lead
    {
        SynthPreset lead;
        lead.name = "Bright Lead";
        lead.synthParams.osc1Wave = WaveType::Saw;
        lead.synthParams.osc1Gain = 0.5f;
        lead.synthParams.osc2Wave = WaveType::Saw;
        lead.synthParams.osc2Gain = 0.5f;
        lead.synthParams.osc2Detune = 7.0f;
        lead.synthParams.filterCutoff = 2000.0f;
        lead.synthParams.filterResonance = 1.5f;
        lead.synthParams.filterEnvAmount = 0.3f;
        lead.synthParams.ampAdsr = { 0.01f, 0.1f, 0.8f, 0.3f };
        lead.synthParams.lfoWave = LfoWaveType::Triangle;
        lead.synthParams.lfoRate = 5.0f;
        lead.synthParams.lfoDepth = 0.3f;
        lead.synthParams.lfoDestination = LfoDestination::Osc2Pitch;
        lead.delayTime = 375.0f;
        lead.delayFeedback = 0.3f;
        lead.delayMix = 0.25f;
        factoryPresets.push_back(lead);
    }
    
    // 4. Pad
    {
        SynthPreset pad;
        pad.name = "Warm Pad";
        pad.synthParams.osc1Wave = WaveType::Saw;
        pad.synthParams.osc1Gain = 0.4f;
        pad.synthParams.osc2Wave = WaveType::Sine;
        pad.synthParams.osc2Gain = 0.4f;
        pad.synthParams.osc2Octave = 1;
        pad.synthParams.filterCutoff = 800.0f;
        pad.synthParams.filterResonance = 0.8f;
        pad.synthParams.filterEnvAmount = 0.2f;
        pad.synthParams.ampAdsr = { 0.5f, 0.3f, 0.7f, 1.0f };
        pad.synthParams.filterAdsr = { 0.8f, 0.5f, 0.5f, 1.0f };
        pad.synthParams.lfoWave = LfoWaveType::Sine;
        pad.synthParams.lfoRate = 0.5f;
        pad.synthParams.lfoDepth = 0.4f;
        pad.synthParams.lfoDestination = LfoDestination::FilterCutoff;
        factoryPresets.push_back(pad);
    }
    
    // 5. Pluck
    {
        SynthPreset pluck;
        pluck.name = "Pluck";
        pluck.synthParams.osc1Wave = WaveType::Square;
        pluck.synthParams.osc1Gain = 0.5f;
        pluck.synthParams.osc2Wave = WaveType::Saw;
        pluck.synthParams.osc2Gain = 0.3f;
        pluck.synthParams.osc2Detune = 5.0f;
        pluck.synthParams.filterCutoff = 3000.0f;
        pluck.synthParams.filterResonance = 1.2f;
        pluck.synthParams.filterEnvAmount = 0.8f;
        pluck.synthParams.ampAdsr = { 0.001f, 0.2f, 0.0f, 0.1f };
        pluck.synthParams.filterAdsr = { 0.001f, 0.15f, 0.0f, 0.1f };
        pluck.delayTime = 200.0f;
        pluck.delayFeedback = 0.4f;
        pluck.delayMix = 0.3f;
        factoryPresets.push_back(pluck);
    }
    
    // 6. Dirty Synth
    {
        SynthPreset dirty;
        dirty.name = "Dirty Synth";
        dirty.synthParams.osc1Wave = WaveType::Square;
        dirty.synthParams.osc1Gain = 0.6f;
        dirty.synthParams.osc2Wave = WaveType::Saw;
        dirty.synthParams.osc2Gain = 0.5f;
        dirty.synthParams.osc2Detune = 15.0f;
        dirty.synthParams.filterCutoff = 1500.0f;
        dirty.synthParams.filterResonance = 3.0f;
        dirty.synthParams.filterEnvAmount = 0.4f;
        dirty.synthParams.ampAdsr = { 0.01f, 0.1f, 0.7f, 0.2f };
        dirty.driveAmount = 0.6f;
        dirty.driveType = DriveType::Fuzz;
        dirty.driveMix = 0.7f;
        factoryPresets.push_back(dirty);
    }

    // 7. Sub Bass - Deep sine sub with square layer
    {
        SynthPreset p;
        p.name = "Sub Bass";
        p.synthParams.osc1Wave = WaveType::Sine;
        p.synthParams.osc1Gain = 0.8f;
        p.synthParams.osc1Octave = -2;
        p.synthParams.osc2Wave = WaveType::Square;
        p.synthParams.osc2Gain = 0.2f;
        p.synthParams.osc2Octave = -1;
        p.synthParams.filterCutoff = 300.0f;
        p.synthParams.filterResonance = 1.0f;
        p.synthParams.filterEnvAmount = 0.3f;
        p.synthParams.ampAdsr = { 0.01f, 0.05f, 1.0f, 0.1f };
        p.synthParams.filterAdsr = { 0.01f, 0.2f, 0.3f, 0.1f };
        factoryPresets.push_back(p);
    }

    // 8. Acid Squelch - Classic resonant filter bass
    {
        SynthPreset p;
        p.name = "Acid Squelch";
        p.synthParams.osc1Wave = WaveType::Saw;
        p.synthParams.osc1Gain = 0.7f;
        p.synthParams.osc2Wave = WaveType::Saw;
        p.synthParams.osc2Gain = 0.0f;
        p.synthParams.filterCutoff = 400.0f;
        p.synthParams.filterResonance = 8.0f;
        p.synthParams.filterEnvAmount = 0.9f;
        p.synthParams.ampAdsr = { 0.001f, 0.15f, 0.0f, 0.05f };
        p.synthParams.filterAdsr = { 0.001f, 0.2f, 0.0f, 0.05f };
        p.driveType = DriveType::Tube;
        p.driveAmount = 0.3f;
        p.driveMix = 0.5f;
        factoryPresets.push_back(p);
    }

    // 9. Ethereal Pad - Slow evolving sine+saw pad with reverb
    {
        SynthPreset p;
        p.name = "Ethereal Pad";
        p.synthParams.osc1Wave = WaveType::Sine;
        p.synthParams.osc1Gain = 0.5f;
        p.synthParams.osc2Wave = WaveType::Saw;
        p.synthParams.osc2Gain = 0.3f;
        p.synthParams.osc2Octave = 1;
        p.synthParams.osc2Detune = 8.0f;
        p.synthParams.filterCutoff = 1200.0f;
        p.synthParams.filterResonance = 0.5f;
        p.synthParams.filterEnvAmount = 0.1f;
        p.synthParams.ampAdsr = { 1.0f, 0.5f, 0.8f, 2.0f };
        p.synthParams.filterAdsr = { 1.5f, 0.8f, 0.6f, 2.0f };
        p.synthParams.lfoWave = LfoWaveType::Sine;
        p.synthParams.lfoRate = 0.3f;
        p.synthParams.lfoDepth = 0.3f;
        p.synthParams.lfoDestination = LfoDestination::FilterCutoff;
        p.reverbRoomSize = 0.9f;
        p.reverbDamping = 0.3f;
        p.reverbWetLevel = 0.6f;
        p.reverbDryLevel = 0.4f;
        p.reverbWidth = 1.0f;
        factoryPresets.push_back(p);
    }

    // 10. Square Lead - Mono-style square lead with vibrato
    {
        SynthPreset p;
        p.name = "Square Lead";
        p.synthParams.osc1Wave = WaveType::Square;
        p.synthParams.osc1Gain = 0.6f;
        p.synthParams.osc2Wave = WaveType::Square;
        p.synthParams.osc2Gain = 0.4f;
        p.synthParams.osc2Octave = 1;
        p.synthParams.filterCutoff = 3500.0f;
        p.synthParams.filterResonance = 1.8f;
        p.synthParams.filterEnvAmount = 0.2f;
        p.synthParams.ampAdsr = { 0.01f, 0.05f, 0.9f, 0.2f };
        p.synthParams.lfoWave = LfoWaveType::Sine;
        p.synthParams.lfoRate = 5.5f;
        p.synthParams.lfoDepth = 0.15f;
        p.synthParams.lfoDestination = LfoDestination::Osc1Pitch;
        p.delayTime = 300.0f;
        p.delayFeedback = 0.25f;
        p.delayMix = 0.2f;
        factoryPresets.push_back(p);
    }

    // 11. Glass Keys - Bright pluck with highpass filter
    {
        SynthPreset p;
        p.name = "Glass Keys";
        p.synthParams.osc1Wave = WaveType::Sine;
        p.synthParams.osc1Gain = 0.6f;
        p.synthParams.osc2Wave = WaveType::Square;
        p.synthParams.osc2Gain = 0.3f;
        p.synthParams.osc2Octave = 2;
        p.synthParams.filterType = FilterType::HighPass;
        p.synthParams.filterCutoff = 600.0f;
        p.synthParams.filterResonance = 2.5f;
        p.synthParams.filterEnvAmount = 0.5f;
        p.synthParams.ampAdsr = { 0.001f, 0.3f, 0.1f, 0.5f };
        p.synthParams.filterAdsr = { 0.001f, 0.4f, 0.0f, 0.3f };
        p.reverbRoomSize = 0.7f;
        p.reverbDamping = 0.4f;
        p.reverbWetLevel = 0.45f;
        p.reverbDryLevel = 0.55f;
        factoryPresets.push_back(p);
    }

    // 12. Detuned Saws - Classic supersaw-style
    {
        SynthPreset p;
        p.name = "Detuned Saws";
        p.synthParams.osc1Wave = WaveType::Saw;
        p.synthParams.osc1Gain = 0.5f;
        p.synthParams.osc2Wave = WaveType::Saw;
        p.synthParams.osc2Gain = 0.5f;
        p.synthParams.osc2Detune = 25.0f;
        p.synthParams.filterCutoff = 4000.0f;
        p.synthParams.filterResonance = 0.7f;
        p.synthParams.filterEnvAmount = 0.15f;
        p.synthParams.ampAdsr = { 0.01f, 0.2f, 0.7f, 0.4f };
        p.reverbRoomSize = 0.6f;
        p.reverbWetLevel = 0.35f;
        p.reverbDryLevel = 0.65f;
        factoryPresets.push_back(p);
    }

    // 13. Wobble Bass - LFO modulating filter cutoff
    {
        SynthPreset p;
        p.name = "Wobble Bass";
        p.synthParams.osc1Wave = WaveType::Saw;
        p.synthParams.osc1Gain = 0.6f;
        p.synthParams.osc1Octave = -1;
        p.synthParams.osc2Wave = WaveType::Square;
        p.synthParams.osc2Gain = 0.4f;
        p.synthParams.osc2Octave = -1;
        p.synthParams.osc2Detune = 5.0f;
        p.synthParams.filterCutoff = 800.0f;
        p.synthParams.filterResonance = 4.0f;
        p.synthParams.filterEnvAmount = 0.2f;
        p.synthParams.ampAdsr = { 0.01f, 0.05f, 1.0f, 0.1f };
        p.synthParams.lfoWave = LfoWaveType::Triangle;
        p.synthParams.lfoRate = 3.0f;
        p.synthParams.lfoDepth = 0.7f;
        p.synthParams.lfoDestination = LfoDestination::FilterCutoff;
        p.driveType = DriveType::Soft;
        p.driveAmount = 0.25f;
        factoryPresets.push_back(p);
    }

    // 14. Organ Tone - Sine layers at different octaves
    {
        SynthPreset p;
        p.name = "Organ Tone";
        p.synthParams.osc1Wave = WaveType::Sine;
        p.synthParams.osc1Gain = 0.6f;
        p.synthParams.osc2Wave = WaveType::Sine;
        p.synthParams.osc2Gain = 0.4f;
        p.synthParams.osc2Octave = 1;
        p.synthParams.filterCutoff = 5000.0f;
        p.synthParams.filterResonance = 0.5f;
        p.synthParams.filterEnvAmount = 0.0f;
        p.synthParams.ampAdsr = { 0.01f, 0.01f, 1.0f, 0.05f };
        p.synthParams.lfoWave = LfoWaveType::Sine;
        p.synthParams.lfoRate = 6.0f;
        p.synthParams.lfoDepth = 0.1f;
        p.synthParams.lfoDestination = LfoDestination::Osc2Gain;
        p.reverbRoomSize = 0.5f;
        p.reverbWetLevel = 0.25f;
        p.reverbDryLevel = 0.75f;
        factoryPresets.push_back(p);
    }

    // 15. Harsh Industrial - Aggressive distorted square
    {
        SynthPreset p;
        p.name = "Harsh Industrial";
        p.synthParams.osc1Wave = WaveType::Square;
        p.synthParams.osc1Gain = 0.7f;
        p.synthParams.osc2Wave = WaveType::Saw;
        p.synthParams.osc2Gain = 0.5f;
        p.synthParams.osc2Detune = 30.0f;
        p.synthParams.filterType = FilterType::BandPass;
        p.synthParams.filterCutoff = 1000.0f;
        p.synthParams.filterResonance = 5.0f;
        p.synthParams.filterEnvAmount = 0.3f;
        p.synthParams.ampAdsr = { 0.001f, 0.08f, 0.8f, 0.15f };
        p.driveType = DriveType::Hard;
        p.driveAmount = 0.8f;
        p.driveMix = 0.8f;
        factoryPresets.push_back(p);
    }

    // 16. Soft Strings - Gentle saw pad with slow attack
    {
        SynthPreset p;
        p.name = "Soft Strings";
        p.synthParams.osc1Wave = WaveType::Saw;
        p.synthParams.osc1Gain = 0.4f;
        p.synthParams.osc2Wave = WaveType::Saw;
        p.synthParams.osc2Gain = 0.4f;
        p.synthParams.osc2Detune = 12.0f;
        p.synthParams.filterCutoff = 2500.0f;
        p.synthParams.filterResonance = 0.4f;
        p.synthParams.filterEnvAmount = -0.2f;
        p.synthParams.ampAdsr = { 0.8f, 0.4f, 0.7f, 1.5f };
        p.synthParams.filterAdsr = { 0.6f, 0.3f, 0.6f, 1.2f };
        p.synthParams.lfoWave = LfoWaveType::Triangle;
        p.synthParams.lfoRate = 0.2f;
        p.synthParams.lfoDepth = 0.2f;
        p.synthParams.lfoDestination = LfoDestination::Osc1Gain;
        p.reverbRoomSize = 0.8f;
        p.reverbDamping = 0.6f;
        p.reverbWetLevel = 0.5f;
        p.reverbDryLevel = 0.5f;
        factoryPresets.push_back(p);
    }

    // 17. Sync Lead - Bright saw lead with fast LFO
    {
        SynthPreset p;
        p.name = "Sync Lead";
        p.synthParams.osc1Wave = WaveType::Saw;
        p.synthParams.osc1Gain = 0.6f;
        p.synthParams.osc2Wave = WaveType::Square;
        p.synthParams.osc2Gain = 0.3f;
        p.synthParams.osc2Octave = 1;
        p.synthParams.osc2Detune = 3.0f;
        p.synthParams.filterCutoff = 5000.0f;
        p.synthParams.filterResonance = 2.0f;
        p.synthParams.filterEnvAmount = 0.4f;
        p.synthParams.ampAdsr = { 0.001f, 0.1f, 0.85f, 0.15f };
        p.synthParams.filterAdsr = { 0.001f, 0.15f, 0.3f, 0.15f };
        p.synthParams.lfoWave = LfoWaveType::SawUp;
        p.synthParams.lfoRate = 8.0f;
        p.synthParams.lfoDepth = 0.2f;
        p.synthParams.lfoDestination = LfoDestination::Osc2Pitch;
        p.delayTime = 250.0f;
        p.delayFeedback = 0.35f;
        p.delayMix = 0.2f;
        factoryPresets.push_back(p);
    }

    // 18. Tape Echo Keys - Square+sine with long delay
    {
        SynthPreset p;
        p.name = "Tape Echo Keys";
        p.synthParams.osc1Wave = WaveType::Square;
        p.synthParams.osc1Gain = 0.4f;
        p.synthParams.osc2Wave = WaveType::Sine;
        p.synthParams.osc2Gain = 0.5f;
        p.synthParams.osc2Octave = 1;
        p.synthParams.filterCutoff = 2200.0f;
        p.synthParams.filterResonance = 1.0f;
        p.synthParams.filterEnvAmount = 0.3f;
        p.synthParams.ampAdsr = { 0.005f, 0.3f, 0.4f, 0.6f };
        p.synthParams.filterAdsr = { 0.005f, 0.25f, 0.2f, 0.5f };
        p.driveType = DriveType::Soft;
        p.driveAmount = 0.15f;
        p.delayTime = 500.0f;
        p.delayFeedback = 0.55f;
        p.delayMix = 0.4f;
        factoryPresets.push_back(p);
    }

    // 19. Nasal Quack - Bandpass filtered square
    {
        SynthPreset p;
        p.name = "Nasal Quack";
        p.synthParams.osc1Wave = WaveType::Square;
        p.synthParams.osc1Gain = 0.7f;
        p.synthParams.osc2Wave = WaveType::Square;
        p.synthParams.osc2Gain = 0.3f;
        p.synthParams.osc2Octave = -1;
        p.synthParams.filterType = FilterType::BandPass;
        p.synthParams.filterCutoff = 800.0f;
        p.synthParams.filterResonance = 6.0f;
        p.synthParams.filterEnvAmount = 0.7f;
        p.synthParams.ampAdsr = { 0.001f, 0.1f, 0.6f, 0.15f };
        p.synthParams.filterAdsr = { 0.001f, 0.12f, 0.1f, 0.1f };
        factoryPresets.push_back(p);
    }

    // 20. Ambient Drone - Very slow evolving texture
    {
        SynthPreset p;
        p.name = "Ambient Drone";
        p.synthParams.osc1Wave = WaveType::Saw;
        p.synthParams.osc1Gain = 0.3f;
        p.synthParams.osc2Wave = WaveType::Sine;
        p.synthParams.osc2Gain = 0.5f;
        p.synthParams.osc2Octave = -1;
        p.synthParams.osc2Detune = 3.0f;
        p.synthParams.filterCutoff = 600.0f;
        p.synthParams.filterResonance = 1.5f;
        p.synthParams.filterEnvAmount = 0.05f;
        p.synthParams.ampAdsr = { 2.0f, 1.0f, 0.9f, 3.0f };
        p.synthParams.filterAdsr = { 2.0f, 1.5f, 0.7f, 3.0f };
        p.synthParams.lfoWave = LfoWaveType::Random;
        p.synthParams.lfoRate = 0.1f;
        p.synthParams.lfoDepth = 0.5f;
        p.synthParams.lfoDestination = LfoDestination::FilterCutoff;
        p.reverbRoomSize = 1.0f;
        p.reverbDamping = 0.2f;
        p.reverbWetLevel = 0.7f;
        p.reverbDryLevel = 0.3f;
        p.reverbWidth = 1.0f;
        factoryPresets.push_back(p);
    }

    // 21. Tube Warmth - Warm driven sine bass
    {
        SynthPreset p;
        p.name = "Tube Warmth";
        p.synthParams.osc1Wave = WaveType::Sine;
        p.synthParams.osc1Gain = 0.7f;
        p.synthParams.osc1Octave = -1;
        p.synthParams.osc2Wave = WaveType::Saw;
        p.synthParams.osc2Gain = 0.2f;
        p.synthParams.osc2Octave = 0;
        p.synthParams.filterCutoff = 1200.0f;
        p.synthParams.filterResonance = 1.2f;
        p.synthParams.filterEnvAmount = 0.4f;
        p.synthParams.ampAdsr = { 0.01f, 0.15f, 0.85f, 0.2f };
        p.synthParams.filterAdsr = { 0.01f, 0.25f, 0.3f, 0.2f };
        p.driveType = DriveType::Tube;
        p.driveAmount = 0.45f;
        p.driveMix = 0.6f;
        factoryPresets.push_back(p);
    }

    // 22. HighPass Sweep - Rising filtered saw pad
    {
        SynthPreset p;
        p.name = "HighPass Sweep";
        p.synthParams.osc1Wave = WaveType::Saw;
        p.synthParams.osc1Gain = 0.5f;
        p.synthParams.osc2Wave = WaveType::Saw;
        p.synthParams.osc2Gain = 0.5f;
        p.synthParams.osc2Detune = 15.0f;
        p.synthParams.osc2Octave = -1;
        p.synthParams.filterType = FilterType::HighPass;
        p.synthParams.filterCutoff = 200.0f;
        p.synthParams.filterResonance = 3.0f;
        p.synthParams.filterEnvAmount = 0.8f;
        p.synthParams.ampAdsr = { 0.3f, 0.5f, 0.6f, 1.0f };
        p.synthParams.filterAdsr = { 0.5f, 0.8f, 0.2f, 0.8f };
        p.reverbRoomSize = 0.7f;
        p.reverbWetLevel = 0.4f;
        p.reverbDryLevel = 0.6f;
        factoryPresets.push_back(p);
    }

    // 23. Bit Crusher - Lo-fi square with hard drive
    {
        SynthPreset p;
        p.name = "Bit Crusher";
        p.synthParams.osc1Wave = WaveType::Square;
        p.synthParams.osc1Gain = 0.5f;
        p.synthParams.osc2Wave = WaveType::Square;
        p.synthParams.osc2Gain = 0.4f;
        p.synthParams.osc2Octave = 1;
        p.synthParams.osc2Detune = 50.0f;
        p.synthParams.filterCutoff = 6000.0f;
        p.synthParams.filterResonance = 1.0f;
        p.synthParams.filterEnvAmount = -0.3f;
        p.synthParams.ampAdsr = { 0.001f, 0.1f, 0.7f, 0.1f };
        p.driveType = DriveType::Hard;
        p.driveAmount = 0.7f;
        p.driveMix = 0.9f;
        factoryPresets.push_back(p);
    }

    // 24. Trance Pluck - Short bright saw pluck with delay
    {
        SynthPreset p;
        p.name = "Trance Pluck";
        p.synthParams.osc1Wave = WaveType::Saw;
        p.synthParams.osc1Gain = 0.5f;
        p.synthParams.osc2Wave = WaveType::Saw;
        p.synthParams.osc2Gain = 0.4f;
        p.synthParams.osc2Detune = 20.0f;
        p.synthParams.filterCutoff = 8000.0f;
        p.synthParams.filterResonance = 1.5f;
        p.synthParams.filterEnvAmount = 0.9f;
        p.synthParams.ampAdsr = { 0.001f, 0.15f, 0.0f, 0.08f };
        p.synthParams.filterAdsr = { 0.001f, 0.1f, 0.0f, 0.08f };
        p.delayTime = 333.0f;
        p.delayFeedback = 0.5f;
        p.delayMix = 0.35f;
        p.reverbRoomSize = 0.4f;
        p.reverbWetLevel = 0.2f;
        p.reverbDryLevel = 0.8f;
        factoryPresets.push_back(p);
    }

    // 25. SawDown LFO Pad - Rhythmic volume swells
    {
        SynthPreset p;
        p.name = "Rhythmic Pad";
        p.synthParams.osc1Wave = WaveType::Saw;
        p.synthParams.osc1Gain = 0.5f;
        p.synthParams.osc2Wave = WaveType::Square;
        p.synthParams.osc2Gain = 0.3f;
        p.synthParams.osc2Detune = 6.0f;
        p.synthParams.filterCutoff = 1800.0f;
        p.synthParams.filterResonance = 1.0f;
        p.synthParams.filterEnvAmount = 0.15f;
        p.synthParams.ampAdsr = { 0.2f, 0.3f, 0.8f, 0.8f };
        p.synthParams.lfoWave = LfoWaveType::SawDown;
        p.synthParams.lfoRate = 2.0f;
        p.synthParams.lfoDepth = 0.6f;
        p.synthParams.lfoDestination = LfoDestination::Osc1Gain;
        p.reverbRoomSize = 0.6f;
        p.reverbWetLevel = 0.35f;
        p.reverbDryLevel = 0.65f;
        factoryPresets.push_back(p);
    }

    // 26. Fuzz Lead - Aggressive fuzz-distorted lead
    {
        SynthPreset p;
        p.name = "Fuzz Lead";
        p.synthParams.osc1Wave = WaveType::Saw;
        p.synthParams.osc1Gain = 0.6f;
        p.synthParams.osc2Wave = WaveType::Sine;
        p.synthParams.osc2Gain = 0.3f;
        p.synthParams.osc2Octave = -1;
        p.synthParams.filterCutoff = 2500.0f;
        p.synthParams.filterResonance = 2.5f;
        p.synthParams.filterEnvAmount = 0.5f;
        p.synthParams.ampAdsr = { 0.01f, 0.08f, 0.9f, 0.25f };
        p.synthParams.filterAdsr = { 0.01f, 0.2f, 0.4f, 0.2f };
        p.driveType = DriveType::Fuzz;
        p.driveAmount = 0.5f;
        p.driveMix = 0.6f;
        p.delayTime = 400.0f;
        p.delayFeedback = 0.3f;
        p.delayMix = 0.15f;
        factoryPresets.push_back(p);
    }
}

#include "MainComponent.h"

MainComponent::MainComponent()
    : synthSource(keyboardState),
      keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard) {
    
    // Set custom look and feel
    setLookAndFeel(&customLookAndFeel);
    
    // Initialize audio device manager
    auto result = deviceManager.initialiseWithDefaultDevices(0, 2);
    if (result.isNotEmpty()) {
        juce::Logger::writeToLog("Audio device initialization error: " + result);
    }
    
    // Setup audio source player
    audioSourcePlayer.setSource(&synthSource);
    deviceManager.addAudioCallback(&audioSourcePlayer);
    
    // Title
    titleLabel.setText(juce::String(juce::CharPointer_UTF8("───  ::ασε~  ─   Alien Sound Engineering")), juce::dontSendNotification);    titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, CustomLookAndFeel::accentColor);
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);
    
    // Create preset component
    presetComponent = std::make_unique<PresetComponent>(presetManager);
    presetComponent->onPresetLoaded = [this](const SynthPreset& preset) { applyPreset(preset); };
    presetComponent->onGetCurrentPreset = [this]() { return getCurrentPreset(); };
    addAndMakeVisible(*presetComponent);
    
    // Create oscillator components
    osc1Component = std::make_unique<OscillatorComponent>("Oscillator 1");
    osc1Component->onParameterChanged = [this] { updateSynthParameters(); };
    addAndMakeVisible(*osc1Component);
    
    osc2Component = std::make_unique<OscillatorComponent>("Oscillator 2");
    osc2Component->onParameterChanged = [this] { updateSynthParameters(); };
    addAndMakeVisible(*osc2Component);
    
    // Create filter component
    filterComponent = std::make_unique<FilterComponent>();
    filterComponent->onParameterChanged = [this] { updateSynthParameters(); };
    addAndMakeVisible(*filterComponent);
    
    // Create ADSR components
    ampAdsrComponent = std::make_unique<AdsrComponent>("Amp ADSR");
    ampAdsrComponent->onParameterChanged = [this] { updateSynthParameters(); };
    addAndMakeVisible(*ampAdsrComponent);
    
    filterAdsrComponent = std::make_unique<AdsrComponent>("Filter ADSR");
    filterAdsrComponent->onParameterChanged = [this] { updateSynthParameters(); };
    addAndMakeVisible(*filterAdsrComponent);
    
    // Create LFO component
    lfoComponent = std::make_unique<LfoComponent>();
    lfoComponent->onParameterChanged = [this] { updateSynthParameters(); };
    addAndMakeVisible(*lfoComponent);
    
    // Create effects component
    effectsComponent = std::make_unique<EffectsComponent>();
    effectsComponent->onParameterChanged = [this] { updateSynthParameters(); };
    addAndMakeVisible(*effectsComponent);
    
    // Master gain slider
    masterGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    masterGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 15);
    masterGainSlider.setRange(0.0, 1.0, 0.01);
    masterGainSlider.setSkewFactorFromMidPoint(0.25);
    masterGainSlider.setValue(0.7);
    masterGainSlider.onValueChange = [this] { updateSynthParameters(); };
    addAndMakeVisible(masterGainSlider);
    
    masterGainLabel.setText("Master", juce::dontSendNotification);
    masterGainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(masterGainLabel);
    
    // Keyboard component
    keyboardComponent.setAvailableRange(36, 96);  // C2 to C7
    addAndMakeVisible(keyboardComponent);
    
    // Apply initial preset
    if (!presetManager.getFactoryPresets().empty()) {
        applyPreset(presetManager.getFactoryPresets()[0]);
    }
    
    // Initial parameter update
    updateSynthParameters();
    
    setSize(900, 650);
}

MainComponent::~MainComponent() {
    // Clear callbacks before destroying components to prevent dangling references
    if (presetComponent) {
        presetComponent->onPresetLoaded = nullptr;
        presetComponent->onGetCurrentPreset = nullptr;
    }
    if (osc1Component) osc1Component->onParameterChanged = nullptr;
    if (osc2Component) osc2Component->onParameterChanged = nullptr;
    if (filterComponent) filterComponent->onParameterChanged = nullptr;
    if (ampAdsrComponent) ampAdsrComponent->onParameterChanged = nullptr;
    if (filterAdsrComponent) filterAdsrComponent->onParameterChanged = nullptr;
    if (lfoComponent) lfoComponent->onParameterChanged = nullptr;
    if (effectsComponent) effectsComponent->onParameterChanged = nullptr;
    
    // Reset all child components before destroying the look and feel
    presetComponent.reset();
    osc1Component.reset();
    osc2Component.reset();
    filterComponent.reset();
    ampAdsrComponent.reset();
    filterAdsrComponent.reset();
    lfoComponent.reset();
    effectsComponent.reset();
    
    setLookAndFeel(nullptr);
    audioSourcePlayer.setSource(nullptr);
    deviceManager.removeAudioCallback(&audioSourcePlayer);
}

void MainComponent::paint(juce::Graphics& g) {
    g.fillAll(CustomLookAndFeel::backgroundDark);
}

void MainComponent::resized() {
    auto bounds = getLocalBounds().reduced(10);
    
    // Title bar area
    auto titleArea = bounds.removeFromTop(35);
    titleLabel.setBounds(titleArea.removeFromLeft(200));
    
    // Preset component on the right of title
    presetComponent->setBounds(titleArea.reduced(5, 3));
    
    bounds.removeFromTop(5);
    
    // Top row: Oscillators
    auto topRow = bounds.removeFromTop(120);
    auto oscWidth = (topRow.getWidth() - 10) / 2;
    osc1Component->setBounds(topRow.removeFromLeft(oscWidth));
    topRow.removeFromLeft(10);
    osc2Component->setBounds(topRow);
    
    bounds.removeFromTop(10);
    
    // Second row: Filter, Filter ADSR, LFO
    auto secondRow = bounds.removeFromTop(120);
    auto secondColWidth = (secondRow.getWidth() - 20) / 3;
    filterComponent->setBounds(secondRow.removeFromLeft(secondColWidth));
    secondRow.removeFromLeft(10);
    filterAdsrComponent->setBounds(secondRow.removeFromLeft(secondColWidth));
    secondRow.removeFromLeft(10);
    lfoComponent->setBounds(secondRow);
    
    bounds.removeFromTop(10);
    
    // Third row: Amp ADSR, Master, Effects
    auto thirdRow = bounds.removeFromTop(120);
    auto adsrWidth = 150;
    auto masterWidth = 80;
    
    ampAdsrComponent->setBounds(thirdRow.removeFromLeft(adsrWidth));
    thirdRow.removeFromLeft(10);
    
    // Master gain
    auto masterArea = thirdRow.removeFromLeft(masterWidth);
    masterGainLabel.setBounds(masterArea.removeFromTop(20));
    masterGainSlider.setBounds(masterArea);
    
    thirdRow.removeFromLeft(10);
    effectsComponent->setBounds(thirdRow);
    
    bounds.removeFromTop(10);
    
    // Keyboard at bottom 
    constexpr int numWhiteKeys = 36;
    auto keyWidth = static_cast<float>(bounds.getWidth()) / static_cast<float>(numWhiteKeys);
    keyboardComponent.setKeyWidth(keyWidth);
    keyboardComponent.setBounds(bounds);
}

void MainComponent::updateSynthParameters() {
    SynthParameters params;
    
    // Oscillator 1
    params.osc1Wave = osc1Component->getWaveType();
    params.osc1Gain = osc1Component->getGain();
    params.osc1Detune = osc1Component->getDetune();
    params.osc1Octave = osc1Component->getOctave();
    
    // Oscillator 2
    params.osc2Wave = osc2Component->getWaveType();
    params.osc2Gain = osc2Component->getGain();
    params.osc2Detune = osc2Component->getDetune();
    params.osc2Octave = osc2Component->getOctave();
    
    // Filter
    params.filterType = filterComponent->getFilterType();
    params.filterCutoff = filterComponent->getCutoff();
    params.filterResonance = filterComponent->getResonance();
    params.filterEnvAmount = filterComponent->getEnvAmount();
    params.filterActive = filterComponent->isFilterActive(); 
    
    // Envelopes
    params.ampAdsr = ampAdsrComponent->getParameters();
    params.filterAdsr = filterAdsrComponent->getParameters();
    
    // LFO
    params.lfoWave = lfoComponent->getWaveType();
    params.lfoRate = lfoComponent->getRate();
    params.lfoDepth = lfoComponent->getDepth();
    params.lfoDestination = lfoComponent->getDestination();
    
    // Master
    params.masterGain = static_cast<float>(masterGainSlider.getValue());
    
    // Update synth
    synthSource.setParameters(params);
    
    // Update effects
    synthSource.setDriveParameters(
        effectsComponent->getDriveType(),
        effectsComponent->getDriveAmount(),
        effectsComponent->getDriveMix(),
        effectsComponent->getDriveOutputGain()
    );
    
    synthSource.setDelayParameters(
        effectsComponent->getDelayTime(),
        effectsComponent->getDelayFeedback(),
        effectsComponent->getDelayMix()
    );

    synthSource.setReverbParameters(
        effectsComponent->getReverbRoomSize(),
        effectsComponent->getReverbDamping(),
        effectsComponent->getReverbWetLevel(),
        effectsComponent->getReverbDryLevel(),
        effectsComponent->getReverbWidth()
    );
}

void MainComponent::applyPreset(const SynthPreset& preset) {
    // Oscillators
    osc1Component->setWaveType(preset.synthParams.osc1Wave);
    osc1Component->setGain(preset.synthParams.osc1Gain);
    osc1Component->setDetune(preset.synthParams.osc1Detune);
    osc1Component->setOctave(preset.synthParams.osc1Octave);
    
    osc2Component->setWaveType(preset.synthParams.osc2Wave);
    osc2Component->setGain(preset.synthParams.osc2Gain);
    osc2Component->setDetune(preset.synthParams.osc2Detune);
    osc2Component->setOctave(preset.synthParams.osc2Octave);
    
    // Filter
    filterComponent->setFilterType(preset.synthParams.filterType);
    filterComponent->setCutoff(preset.synthParams.filterCutoff);
    filterComponent->setResonance(preset.synthParams.filterResonance);
    filterComponent->setEnvAmount(preset.synthParams.filterEnvAmount);
    
    // Envelopes
    ampAdsrComponent->setParameters(preset.synthParams.ampAdsr);
    filterAdsrComponent->setParameters(preset.synthParams.filterAdsr);
    
    // LFO
    lfoComponent->setWaveType(preset.synthParams.lfoWave);
    lfoComponent->setRate(preset.synthParams.lfoRate);
    lfoComponent->setDepth(preset.synthParams.lfoDepth);
    lfoComponent->setDestination(preset.synthParams.lfoDestination);
    
    // Master
    masterGainSlider.setValue(preset.synthParams.masterGain, juce::dontSendNotification);
    
    // Effects
    effectsComponent->setDriveType(preset.driveType);
    effectsComponent->setDriveAmount(preset.driveAmount);
    effectsComponent->setDriveMix(preset.driveMix);
    effectsComponent->setDriveOutputGain(preset.driveOutputGain);
    effectsComponent->setDelayTime(preset.delayTime);
    effectsComponent->setDelayFeedback(preset.delayFeedback);
    effectsComponent->setDelayMix(preset.delayMix);
    effectsComponent->setReverbRoomSize(preset.reverbRoomSize);
    effectsComponent->setReverbDamping(preset.reverbDamping);
    effectsComponent->setReverbWetLevel(preset.reverbWetLevel);
    effectsComponent->setReverbDryLevel(preset.reverbDryLevel);
    effectsComponent->setReverbWidth(preset.reverbWidth);
    
    // Update synth with new parameters
    updateSynthParameters();
}

SynthPreset MainComponent::getCurrentPreset() {
    SynthPreset preset;
    preset.name = "User Preset";
    
    // Oscillators
    preset.synthParams.osc1Wave = osc1Component->getWaveType();
    preset.synthParams.osc1Gain = osc1Component->getGain();
    preset.synthParams.osc1Detune = osc1Component->getDetune();
    preset.synthParams.osc1Octave = osc1Component->getOctave();
    
    preset.synthParams.osc2Wave = osc2Component->getWaveType();
    preset.synthParams.osc2Gain = osc2Component->getGain();
    preset.synthParams.osc2Detune = osc2Component->getDetune();
    preset.synthParams.osc2Octave = osc2Component->getOctave();
    
    // Filter
    preset.synthParams.filterType = filterComponent->getFilterType();
    preset.synthParams.filterCutoff = filterComponent->getCutoff();
    preset.synthParams.filterResonance = filterComponent->getResonance();
    preset.synthParams.filterEnvAmount = filterComponent->getEnvAmount();
    
    // Envelopes
    preset.synthParams.ampAdsr = ampAdsrComponent->getParameters();
    preset.synthParams.filterAdsr = filterAdsrComponent->getParameters();
    
    // LFO
    preset.synthParams.lfoWave = lfoComponent->getWaveType();
    preset.synthParams.lfoRate = lfoComponent->getRate();
    preset.synthParams.lfoDepth = lfoComponent->getDepth();
    preset.synthParams.lfoDestination = lfoComponent->getDestination();
    
    // Master
    preset.synthParams.masterGain = static_cast<float>(masterGainSlider.getValue());
    
    // Effects
    preset.driveType = effectsComponent->getDriveType();
    preset.driveAmount = effectsComponent->getDriveAmount();
    preset.driveMix = effectsComponent->getDriveMix();
    preset.driveOutputGain = effectsComponent->getDriveOutputGain();
    preset.delayTime = effectsComponent->getDelayTime();
    preset.delayFeedback = effectsComponent->getDelayFeedback();
    preset.delayMix = effectsComponent->getDelayMix();
    preset.reverbRoomSize = effectsComponent->getReverbRoomSize();
    preset.reverbDamping = effectsComponent->getReverbDamping();
    preset.reverbWetLevel = effectsComponent->getReverbWetLevel();
    preset.reverbDryLevel = effectsComponent->getReverbDryLevel();
    preset.reverbWidth = effectsComponent->getReverbWidth();
    
    return preset;
}

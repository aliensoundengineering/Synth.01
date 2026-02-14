#include "PluginEditor.h"

CodingSoundsSynthEditor::CodingSoundsSynthEditor(CodingSoundsSynthProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      keyboardComponent(p.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard) {

    setLookAndFeel(&customLookAndFeel);

    // Title
    titleLabel.setText(":#/ GSLF CodingSounds Synth", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, CustomLookAndFeel::accentColor);
    titleLabel.setJustificationType(juce::Justification::centredLeft);
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
    ampAdsrComponent = std::make_unique<AdsrComponent>("Amp Envelope");
    ampAdsrComponent->onParameterChanged = [this] { updateSynthParameters(); };
    addAndMakeVisible(*ampAdsrComponent);

    filterAdsrComponent = std::make_unique<AdsrComponent>("Filter Envelope");
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
    masterGainSlider.setValue(0.7);
    masterGainSlider.onValueChange = [this] { updateSynthParameters(); };
    addAndMakeVisible(masterGainSlider);

    masterGainLabel.setText("Master", juce::dontSendNotification);
    masterGainLabel.setJustificationType(juce::Justification::centred);
    masterGainLabel.setColour(juce::Label::textColourId, CustomLookAndFeel::accentColor);
    addAndMakeVisible(masterGainLabel);

    // Keyboard component
    keyboardComponent.setAvailableRange(36, 96);
    addAndMakeVisible(keyboardComponent);

    // Apply initial preset
    if (!presetManager.getFactoryPresets().empty()) {
        applyPreset(presetManager.getFactoryPresets()[0]);
    }

    updateSynthParameters();

    setSize(1000, 700);
}

CodingSoundsSynthEditor::~CodingSoundsSynthEditor() {
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
}

void CodingSoundsSynthEditor::paint(juce::Graphics& g) {
    g.fillAll(CustomLookAndFeel::backgroundDark);

    // Subtle separator line under header
    g.setColour(CustomLookAndFeel::backgroundLight.withAlpha(0.3f));
    g.drawHorizontalLine(38, 12.0f, static_cast<float>(getWidth() - 12));
}

void CodingSoundsSynthEditor::resized() {
    auto bounds = getLocalBounds().reduced(8);

    const int gap = 6;
    const int headerH = 30;
    const int keyboardH = 65;

    // --- Header: title left, preset right ---
    auto header = bounds.removeFromTop(headerH);
    titleLabel.setBounds(header.removeFromLeft(260));
    presetComponent->setBounds(header.reduced(0, 2));

    bounds.removeFromTop(gap);

    // Calculate available height for 3 content rows
    int availableH = bounds.getHeight() - keyboardH - gap * 3;
    int fullRow3H = availableH - 120 - 130;       // what effects would get at old sizes
    int row3H = static_cast<int>(fullRow3H * 0.6f); // reduce effects height by 40%
    int recovered = fullRow3H - row3H;            // redistribute freed space
    int row1H = 120 + recovered / 2;              // Osc row gets half
    int row2H = 130 + recovered - recovered / 2;  // Filter/Envelopes row gets the rest

    // --- Row 1: Osc1 | Osc2 | LFO | Master ---
    auto row1 = bounds.removeFromTop(row1H);
    int masterW = 82;
    int lfoW = 180;
    int oscTotalW = row1.getWidth() - masterW - lfoW - gap * 3;
    int oscW = oscTotalW / 2;

    osc1Component->setBounds(row1.removeFromLeft(oscW));
    row1.removeFromLeft(gap);
    osc2Component->setBounds(row1.removeFromLeft(oscW));
    row1.removeFromLeft(gap);
    lfoComponent->setBounds(row1.removeFromLeft(lfoW));
    row1.removeFromLeft(gap);

    // Master knob area
    auto masterArea = row1;
    masterGainLabel.setBounds(masterArea.removeFromTop(20));
    masterGainSlider.setBounds(masterArea.reduced(2, 0));

    bounds.removeFromTop(gap);

    // --- Row 2: Filter | Filter ADSR | Amp ADSR ---
    auto row2 = bounds.removeFromTop(row2H);
    int filterW = static_cast<int>(row2.getWidth() * 0.38f);
    int envelopeW = row2.getWidth() - filterW - gap * 2;
    int envHalfW = envelopeW / 2;

    filterComponent->setBounds(row2.removeFromLeft(filterW));
    row2.removeFromLeft(gap);
    filterAdsrComponent->setBounds(row2.removeFromLeft(envHalfW));
    row2.removeFromLeft(gap);
    ampAdsrComponent->setBounds(row2);

    bounds.removeFromTop(gap);

    // --- Row 3: Effects (full width, taller for reverb) ---
    auto row3 = bounds.removeFromTop(row3H);
    effectsComponent->setBounds(row3);

    bounds.removeFromTop(gap);

    // --- Bottom: Keyboard ---
    auto kbBounds = bounds;
    if (kbBounds.getHeight() > keyboardH)
        kbBounds = kbBounds.removeFromTop(keyboardH);
    constexpr int numWhiteKeys = 36;
    auto keyWidth = static_cast<float>(kbBounds.getWidth()) / static_cast<float>(numWhiteKeys);
    keyboardComponent.setKeyWidth(keyWidth);
    keyboardComponent.setBounds(kbBounds);
}

void CodingSoundsSynthEditor::updateSynthParameters() {
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

    // Update processor
    processorRef.setParameters(params);

    // Update effects
    processorRef.setDriveParameters(
        effectsComponent->getDriveType(),
        effectsComponent->getDriveAmount(),
        effectsComponent->getDriveMix(),
        effectsComponent->getDriveOutputGain()
    );

    processorRef.setDelayParameters(
        effectsComponent->getDelayTime(),
        effectsComponent->getDelayFeedback(),
        effectsComponent->getDelayMix()
    );

    processorRef.setReverbParameters(
        effectsComponent->getReverbRoomSize(),
        effectsComponent->getReverbDamping(),
        effectsComponent->getReverbWetLevel(),
        effectsComponent->getReverbDryLevel(),
        effectsComponent->getReverbWidth()
    );

    // Update effect active states
    processorRef.setDriveActive(effectsComponent->isDriveActive());
    processorRef.setDelayActive(effectsComponent->isDelayActive());
    processorRef.setReverbActive(effectsComponent->isReverbActive());
}

void CodingSoundsSynthEditor::applyPreset(const SynthPreset& preset) {
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

    updateSynthParameters();
}

SynthPreset CodingSoundsSynthEditor::getCurrentPreset() {
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
    preset.synthParams.filterActive = filterComponent->isFilterActive();

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

#include "EffectsComponent.h"
#include "CustomLookAndFeel.h"

EffectsComponent::EffectsComponent() {
    // Drive type selector
    driveTypeSelector.addItem("Soft", 1);
    driveTypeSelector.addItem("Hard", 2);
    driveTypeSelector.addItem("Tube", 3);
    driveTypeSelector.addItem("Fuzz", 4);
    driveTypeSelector.setSelectedId(1);
    driveTypeSelector.onChange = [this] { if (onParameterChanged) onParameterChanged(); };
    addAndMakeVisible(driveTypeSelector);

    // Toggle buttons
    driveActiveButton.setToggleState(true, juce::dontSendNotification);
    driveActiveButton.onClick = [this] { if (onParameterChanged) onParameterChanged(); };
    addAndMakeVisible(driveActiveButton);

    delayActiveButton.setToggleState(true, juce::dontSendNotification);
    delayActiveButton.onClick = [this] { if (onParameterChanged) onParameterChanged(); };
    addAndMakeVisible(delayActiveButton);

    reverbActiveButton.setToggleState(true, juce::dontSendNotification);
    reverbActiveButton.onClick = [this] { if (onParameterChanged) onParameterChanged(); };
    addAndMakeVisible(reverbActiveButton);

    // Drive sliders
    setupSlider(driveAmountSlider, driveAmountLabel, "Drive");
    driveAmountSlider.setRange(0.0, 1.0, 0.01);
    driveAmountSlider.setSkewFactorFromMidPoint(0.25);
    driveAmountSlider.setValue(0.0);

    setupSlider(driveMixSlider, driveMixLabel, "Mix");
    driveMixSlider.setRange(0.0, 1.0, 0.01);
    driveMixSlider.setValue(1.0);

    setupSlider(driveOutputSlider, driveOutputLabel, "Output");
    driveOutputSlider.setRange(0.0, 1.0, 0.01);
    driveOutputSlider.setSkewFactorFromMidPoint(0.25);
    driveOutputSlider.setValue(0.7);

    // Delay sliders
    setupSlider(delayTimeSlider, delayTimeLabel, "Time");
    delayTimeSlider.setRange(1.0, 1000.0, 1.0);
    delayTimeSlider.setValue(250.0);
    delayTimeSlider.setTextValueSuffix(" ms");
    delayTimeSlider.setSkewFactorFromMidPoint(200.0);

    setupSlider(delayFeedbackSlider, delayFeedbackLabel, "Fdbk");
    delayFeedbackSlider.setRange(0.0, 0.95, 0.01);
    delayFeedbackSlider.setValue(0.4);

    setupSlider(delayMixSlider, delayMixLabel, "Mix");
    delayMixSlider.setRange(0.0, 1.0, 0.01);
    delayMixSlider.setValue(0.3);

    // Reverb sliders
    setupSlider(reverbRoomSizeSlider, reverbRoomSizeLabel, "Room");
    reverbRoomSizeSlider.setRange(0.0, 1.0, 0.01);
    reverbRoomSizeSlider.setValue(0.5);

    setupSlider(reverbDampingSlider, reverbDampingLabel, "Damp");
    reverbDampingSlider.setRange(0.0, 1.0, 0.01);
    reverbDampingSlider.setValue(0.5);

    setupSlider(reverbWetSlider, reverbWetLabel, "Wet");
    reverbWetSlider.setRange(0.0, 1.0, 0.01);
    reverbWetSlider.setValue(0.3);

    setupSlider(reverbDrySlider, reverbDryLabel, "Dry"); 
    reverbDrySlider.setRange(0.0, 1.0, 0.01);
    reverbDrySlider.setValue(0.7);

    setupSlider(reverbWidthSlider, reverbWidthLabel, "Width");
    reverbWidthSlider.setRange(0.0, 1.0, 0.01);
    reverbWidthSlider.setValue(1.0);
}

void EffectsComponent::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text) {
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 14);
    slider.onValueChange = [this] { if (onParameterChanged) onParameterChanged(); };
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void EffectsComponent::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    g.setColour(CustomLookAndFeel::backgroundMid.withAlpha(0.5f));
    g.fillRoundedRectangle(bounds, 6.0f);

    g.setColour(CustomLookAndFeel::backgroundLight.withAlpha(0.6f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 6.0f, 1.0f);

    // Calculate section widths based on uniform knob size (3+3+5 = 11 knobs)
    float totalW = bounds.getWidth() - 12.0f; // reduced(6) padding
    float knobUnit = totalW / 11.0f;
    float driveW = knobUnit * 3.0f;
    float delayW = knobUnit * 3.0f;

    float divider1X = 6.0f + driveW + 4.0f;   // after drive section + inner padding
    float divider2X = divider1X + delayW + 4.0f; // after delay section

    // Section dividers
    g.setColour(CustomLookAndFeel::backgroundLight.withAlpha(0.4f));
    g.drawVerticalLine(static_cast<int>(divider1X), 6.0f, bounds.getHeight() - 6.0f);
    g.drawVerticalLine(static_cast<int>(divider2X), 6.0f, bounds.getHeight() - 6.0f);

    // Section titles
    g.setColour(CustomLookAndFeel::accentColor);
    g.setFont(13.0f);
    g.drawText("Drive", 10, 6, 50, 18, juce::Justification::left);
    g.drawText("Delay", static_cast<int>(divider1X) + 10, 6, 50, 18, juce::Justification::left);
    g.drawText("Reverb", static_cast<int>(divider2X) + 10, 6, 60, 18, juce::Justification::left);
}

void EffectsComponent::resized() {
    auto area = getLocalBounds().reduced(6);
    const int headerH = 26;
    const int labelH = 14;
    const int toggleW = 28;
    const int toggleH = 16;

    // Uniform knob width: total 11 knobs (Drive=3, Delay=3, Reverb=5)
    // Subtract padding between sections (4px inner padding each side × 3 sections = ~24px)
    int totalKnobSpace = area.getWidth() - 24; // reserve for inner padding
    int kW = totalKnobSpace / 11;

    // Section widths based on number of knobs
    int driveSecW = kW * 3 + 8;   // 3 knobs + 8px inner padding
    int delaySecW = kW * 3 + 8;   // 3 knobs + 8px inner padding
    // Reverb gets remaining space (5 knobs)

    // --- DRIVE section ---
    {
        auto section = area.removeFromLeft(driveSecW).reduced(4, 0);
        auto header = section.removeFromTop(headerH);
        header.removeFromLeft(54); // skip "Drive" title
        driveActiveButton.setBounds(header.removeFromLeft(toggleW).reduced(0, (headerH - toggleH) / 2));
        header.removeFromLeft(4);
        driveTypeSelector.setBounds(header.reduced(0, 3));

        section.removeFromTop(2);

        // 3 knobs, each kW wide
        auto k1 = section.removeFromLeft(kW);
        driveAmountLabel.setBounds(k1.removeFromTop(labelH));
        driveAmountSlider.setBounds(k1);

        auto k2 = section.removeFromLeft(kW);
        driveMixLabel.setBounds(k2.removeFromTop(labelH));
        driveMixSlider.setBounds(k2);

        driveOutputLabel.setBounds(section.removeFromTop(labelH));
        driveOutputSlider.setBounds(section);
    }

    // --- DELAY section ---
    {
        auto section = area.removeFromLeft(delaySecW).reduced(4, 0);
        auto header = section.removeFromTop(headerH);
        header.removeFromLeft(54); // skip "Delay" title
        delayActiveButton.setBounds(header.removeFromLeft(toggleW).reduced(0, (headerH - toggleH) / 2));

        section.removeFromTop(2);

        auto k1 = section.removeFromLeft(kW);
        delayTimeLabel.setBounds(k1.removeFromTop(labelH));
        delayTimeSlider.setBounds(k1);

        auto k2 = section.removeFromLeft(kW);
        delayFeedbackLabel.setBounds(k2.removeFromTop(labelH));
        delayFeedbackSlider.setBounds(k2);

        delayMixLabel.setBounds(section.removeFromTop(labelH));
        delayMixSlider.setBounds(section);
    }

    // --- REVERB section: 5 knobs, same kW size as drive/delay ---
    {
        auto section = area.reduced(4, 0);
        auto header = section.removeFromTop(headerH);
        header.removeFromLeft(64); // skip "Reverb" title
        reverbActiveButton.setBounds(header.removeFromLeft(toggleW).reduced(0, (headerH - toggleH) / 2));

        section.removeFromTop(2);

        auto rk1 = section.removeFromLeft(kW);
        reverbRoomSizeLabel.setBounds(rk1.removeFromTop(labelH));
        reverbRoomSizeSlider.setBounds(rk1);

        auto rk2 = section.removeFromLeft(kW);
        reverbDampingLabel.setBounds(rk2.removeFromTop(labelH));
        reverbDampingSlider.setBounds(rk2);

        auto rk3 = section.removeFromLeft(kW);
        reverbWetLabel.setBounds(rk3.removeFromTop(labelH));
        reverbWetSlider.setBounds(rk3);

        auto rk4 = section.removeFromLeft(kW);
        reverbDryLabel.setBounds(rk4.removeFromTop(labelH));
        reverbDrySlider.setBounds(rk4);

        reverbWidthLabel.setBounds(section.removeFromTop(labelH));
        reverbWidthSlider.setBounds(section);
    }
}

DriveType EffectsComponent::getDriveType() const {
    switch (driveTypeSelector.getSelectedId()) {
        case 1: return DriveType::Soft;
        case 2: return DriveType::Hard;
        case 3: return DriveType::Tube;
        case 4: return DriveType::Fuzz;
        default: return DriveType::Soft;
    }
}

float EffectsComponent::getDriveAmount() const { return static_cast<float>(driveAmountSlider.getValue()); }
float EffectsComponent::getDriveMix() const { return static_cast<float>(driveMixSlider.getValue()); }
float EffectsComponent::getDriveOutputGain() const { return static_cast<float>(driveOutputSlider.getValue()); }
float EffectsComponent::getDelayTime() const { return static_cast<float>(delayTimeSlider.getValue()); }
float EffectsComponent::getDelayFeedback() const { return static_cast<float>(delayFeedbackSlider.getValue()); }
float EffectsComponent::getDelayMix() const { return static_cast<float>(delayMixSlider.getValue()); }

float EffectsComponent::getReverbRoomSize() const { return static_cast<float>(reverbRoomSizeSlider.getValue()); }
float EffectsComponent::getReverbDamping() const { return static_cast<float>(reverbDampingSlider.getValue()); }
float EffectsComponent::getReverbWetLevel() const { return static_cast<float>(reverbWetSlider.getValue()); }
float EffectsComponent::getReverbDryLevel() const { return static_cast<float>(reverbDrySlider.getValue()); }
float EffectsComponent::getReverbWidth() const { return static_cast<float>(reverbWidthSlider.getValue()); }

void EffectsComponent::setDriveType(DriveType type) {
    int id = 1;
    switch (type) {
        case DriveType::Soft: id = 1; break;
        case DriveType::Hard: id = 2; break;
        case DriveType::Tube: id = 3; break;
        case DriveType::Fuzz: id = 4; break;
    }
    driveTypeSelector.setSelectedId(id, juce::dontSendNotification);
}

void EffectsComponent::setDriveAmount(float amount) { driveAmountSlider.setValue(amount, juce::dontSendNotification); }
void EffectsComponent::setDriveMix(float mix) { driveMixSlider.setValue(mix, juce::dontSendNotification); }
void EffectsComponent::setDriveOutputGain(float gain) { driveOutputSlider.setValue(gain, juce::dontSendNotification); }
void EffectsComponent::setDelayTime(float time) { delayTimeSlider.setValue(time, juce::dontSendNotification); }
void EffectsComponent::setDelayFeedback(float feedback) { delayFeedbackSlider.setValue(feedback, juce::dontSendNotification); }
void EffectsComponent::setDelayMix(float mix) { delayMixSlider.setValue(mix, juce::dontSendNotification); }

void EffectsComponent::setReverbRoomSize(float size) { reverbRoomSizeSlider.setValue(size, juce::dontSendNotification); }
void EffectsComponent::setReverbDamping(float damping) { reverbDampingSlider.setValue(damping, juce::dontSendNotification); }
void EffectsComponent::setReverbWetLevel(float wet) { reverbWetSlider.setValue(wet, juce::dontSendNotification); }
void EffectsComponent::setReverbDryLevel(float dry) { reverbDrySlider.setValue(dry, juce::dontSendNotification); }
void EffectsComponent::setReverbWidth(float width) { reverbWidthSlider.setValue(width, juce::dontSendNotification); }

// Active state getters
bool EffectsComponent::isDriveActive() const { return driveActiveButton.getToggleState(); }
bool EffectsComponent::isDelayActive() const { return delayActiveButton.getToggleState(); }
bool EffectsComponent::isReverbActive() const { return reverbActiveButton.getToggleState(); }

// Active state setters
void EffectsComponent::setDriveActive(bool active) { driveActiveButton.setToggleState(active, juce::dontSendNotification); }
void EffectsComponent::setDelayActive(bool active) { delayActiveButton.setToggleState(active, juce::dontSendNotification); }
void EffectsComponent::setReverbActive(bool active) { reverbActiveButton.setToggleState(active, juce::dontSendNotification); }

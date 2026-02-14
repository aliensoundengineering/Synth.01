#include "OscillatorComponent.h"
#include "CustomLookAndFeel.h"

OscillatorComponent::OscillatorComponent(const juce::String& name)
    : oscName(name) {

    // Wave selector
    waveSelector.addItem("Sine", 1);
    waveSelector.addItem("Saw", 2);
    waveSelector.addItem("Square", 3);
    waveSelector.setSelectedId(2);
    waveSelector.onChange = [this] { if (onParameterChanged) onParameterChanged(); };
    addAndMakeVisible(waveSelector);

    waveLabel.setText("Wave", juce::dontSendNotification);
    waveLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(waveLabel);

    // Gain slider
    setupSlider(gainSlider, gainLabel, "Gain");
    gainSlider.setRange(0.0, 1.0, 0.01);
    gainSlider.setValue(0.5);

    // Detune slider
    setupSlider(detuneSlider, detuneLabel, "Detune");
    detuneSlider.setRange(-100.0, 100.0, 1.0);
    detuneSlider.setValue(0.0);
    detuneSlider.setTextValueSuffix(" ct");

    // Octave slider
    setupSlider(octaveSlider, octaveLabel, "Octave");
    octaveSlider.setRange(-2, 2, 1);
    octaveSlider.setValue(0);
}

void OscillatorComponent::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text) {
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 56, 14);
    slider.onValueChange = [this] { if (onParameterChanged) onParameterChanged(); };
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void OscillatorComponent::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    g.setColour(CustomLookAndFeel::backgroundMid.withAlpha(0.5f));
    g.fillRoundedRectangle(bounds, 6.0f);

    g.setColour(CustomLookAndFeel::backgroundLight.withAlpha(0.6f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 6.0f, 1.0f);

    // Title
    g.setColour(CustomLookAndFeel::accentColor);
    g.setFont(13.0f);
    g.drawText(oscName, 10, 6, 120, 18, juce::Justification::left);
}

void OscillatorComponent::resized() {
    auto bounds = getLocalBounds().reduced(6);

    // Header row: title on left (painted), wave selector on right
    auto headerRow = bounds.removeFromTop(24);
    headerRow.removeFromLeft(90); // skip title area (compact)
    waveLabel.setBounds(headerRow.removeFromLeft(34));
    headerRow.removeFromLeft(3);
    waveSelector.setBounds(headerRow.reduced(0, 2));

    bounds.removeFromTop(2);

    // Three knobs in equal columns
    const int labelH = 14;
    int knobW = bounds.getWidth() / 3;

    auto gainArea = bounds.removeFromLeft(knobW);
    gainLabel.setBounds(gainArea.removeFromTop(labelH));
    gainSlider.setBounds(gainArea);

    auto detuneArea = bounds.removeFromLeft(knobW);
    detuneLabel.setBounds(detuneArea.removeFromTop(labelH));
    detuneSlider.setBounds(detuneArea);

    octaveLabel.setBounds(bounds.removeFromTop(labelH));
    octaveSlider.setBounds(bounds);
}

WaveType OscillatorComponent::getWaveType() const {
    switch (waveSelector.getSelectedId()) {
        case 1: return WaveType::Sine;
        case 3: return WaveType::Square;
        default: return WaveType::Saw;
    }
}

float OscillatorComponent::getGain() const {
    return static_cast<float>(gainSlider.getValue());
}

float OscillatorComponent::getDetune() const {
    return static_cast<float>(detuneSlider.getValue());
}

int OscillatorComponent::getOctave() const {
    return static_cast<int>(octaveSlider.getValue());
}

void OscillatorComponent::setWaveType(WaveType type) {
    switch (type) {
        case WaveType::Sine: waveSelector.setSelectedId(1, juce::dontSendNotification); break;
        case WaveType::Saw: waveSelector.setSelectedId(2, juce::dontSendNotification); break;
        case WaveType::Square: waveSelector.setSelectedId(3, juce::dontSendNotification); break;
    }
}

void OscillatorComponent::setGain(float gain) {
    gainSlider.setValue(gain, juce::dontSendNotification);
}

void OscillatorComponent::setDetune(float cents) {
    detuneSlider.setValue(cents, juce::dontSendNotification);
}

void OscillatorComponent::setOctave(int octave) {
    octaveSlider.setValue(octave, juce::dontSendNotification);
}

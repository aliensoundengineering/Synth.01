#include "LfoComponent.h"
#include "CustomLookAndFeel.h"

LfoComponent::LfoComponent() {
    // Wave selector
    waveSelector.addItem("Sine", 1);
    waveSelector.addItem("Triangle", 2);
    waveSelector.addItem("Square", 3);
    waveSelector.addItem("Saw Up", 4);
    waveSelector.addItem("Saw Down", 5);
    waveSelector.addItem("Random", 6);
    waveSelector.setSelectedId(1);
    waveSelector.onChange = [this] { if (onParameterChanged) onParameterChanged(); };
    addAndMakeVisible(waveSelector);

    waveLabel.setText("Wave", juce::dontSendNotification);
    waveLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(waveLabel);

    // Destination selector
    destinationSelector.addItem("None", 1);
    destinationSelector.addItem("Filter", 2);
    destinationSelector.addItem("Osc1 Pitch", 3);
    destinationSelector.addItem("Osc2 Pitch", 4);
    destinationSelector.addItem("Osc1 Gain", 5);
    destinationSelector.addItem("Osc2 Gain", 6);
    destinationSelector.setSelectedId(2);
    destinationSelector.onChange = [this] { if (onParameterChanged) onParameterChanged(); };
    addAndMakeVisible(destinationSelector);

    destLabel.setText("Dest", juce::dontSendNotification);
    destLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(destLabel);

    // Rate slider
    setupSlider(rateSlider, rateLabel, "Rate");
    rateSlider.setRange(0.1, 20.0, 0.1);
    rateSlider.setValue(1.0);
    rateSlider.setSkewFactorFromMidPoint(3.0);
    rateSlider.setTextValueSuffix(" Hz");

    // Depth slider
    setupSlider(depthSlider, depthLabel, "Depth");
    depthSlider.setRange(0.0, 1.0, 0.01);
    depthSlider.setValue(0.5);
}

void LfoComponent::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text) {
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 56, 14);
    slider.onValueChange = [this] { if (onParameterChanged) onParameterChanged(); };
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void LfoComponent::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    g.setColour(CustomLookAndFeel::backgroundMid.withAlpha(0.5f));
    g.fillRoundedRectangle(bounds, 6.0f);

    g.setColour(CustomLookAndFeel::backgroundLight.withAlpha(0.6f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 6.0f, 1.0f);

    g.setColour(CustomLookAndFeel::accentColor);
    g.setFont(13.0f);
    g.drawText("LFO", 10, 6, 40, 18, juce::Justification::left);
}

void LfoComponent::resized() {
    auto bounds = getLocalBounds().reduced(8);

    // Header row 1: title (painted) | Wave selector
    auto headerRow1 = bounds.removeFromTop(22);
    headerRow1.removeFromLeft(44); // skip "LFO" title
    waveLabel.setBounds(headerRow1.removeFromLeft(36));
    headerRow1.removeFromLeft(2);
    waveSelector.setBounds(headerRow1.reduced(0, 1));

    bounds.removeFromTop(2);

    // Header row 2: Destination selector
    auto headerRow2 = bounds.removeFromTop(22);
    headerRow2.removeFromLeft(44);
    destLabel.setBounds(headerRow2.removeFromLeft(36));
    headerRow2.removeFromLeft(2);
    destinationSelector.setBounds(headerRow2.reduced(0, 1));

    bounds.removeFromTop(4);

    // Two knobs in equal columns
    const int labelH = 16;
    int knobW = bounds.getWidth() / 2;

    auto rateArea = bounds.removeFromLeft(knobW);
    rateLabel.setBounds(rateArea.removeFromTop(labelH));
    rateSlider.setBounds(rateArea);

    depthLabel.setBounds(bounds.removeFromTop(labelH));
    depthSlider.setBounds(bounds);
}

LfoWaveType LfoComponent::getWaveType() const {
    switch (waveSelector.getSelectedId()) {
        case 1: return LfoWaveType::Sine;
        case 2: return LfoWaveType::Triangle;
        case 3: return LfoWaveType::Square;
        case 4: return LfoWaveType::SawUp;
        case 5: return LfoWaveType::SawDown;
        case 6: return LfoWaveType::Random;
        default: return LfoWaveType::Sine;
    }
}

float LfoComponent::getRate() const {
    return static_cast<float>(rateSlider.getValue());
}

float LfoComponent::getDepth() const {
    return static_cast<float>(depthSlider.getValue());
}

LfoDestination LfoComponent::getDestination() const {
    switch (destinationSelector.getSelectedId()) {
        case 1: return LfoDestination::None;
        case 2: return LfoDestination::FilterCutoff;
        case 3: return LfoDestination::Osc1Pitch;
        case 4: return LfoDestination::Osc2Pitch;
        case 5: return LfoDestination::Osc1Gain;
        case 6: return LfoDestination::Osc2Gain;
        default: return LfoDestination::FilterCutoff;
    }
}

void LfoComponent::setWaveType(LfoWaveType type) {
    int id = 1;
    switch (type) {
        case LfoWaveType::Sine: id = 1; break;
        case LfoWaveType::Triangle: id = 2; break;
        case LfoWaveType::Square: id = 3; break;
        case LfoWaveType::SawUp: id = 4; break;
        case LfoWaveType::SawDown: id = 5; break;
        case LfoWaveType::Random: id = 6; break;
    }
    waveSelector.setSelectedId(id, juce::dontSendNotification);
}

void LfoComponent::setRate(float rate) {
    rateSlider.setValue(rate, juce::dontSendNotification);
}

void LfoComponent::setDepth(float depth) {
    depthSlider.setValue(depth, juce::dontSendNotification);
}

void LfoComponent::setDestination(LfoDestination dest) {
    int id = 2;
    switch (dest) {
        case LfoDestination::None: id = 1; break;
        case LfoDestination::FilterCutoff: id = 2; break;
        case LfoDestination::Osc1Pitch: id = 3; break;
        case LfoDestination::Osc2Pitch: id = 4; break;
        case LfoDestination::Osc1Gain: id = 5; break;
        case LfoDestination::Osc2Gain: id = 6; break;
    }
    destinationSelector.setSelectedId(id, juce::dontSendNotification);
}

#include "FilterComponent.h"
#include "CustomLookAndFeel.h"

FilterComponent::FilterComponent() {
    // Type selector
    typeSelector.addItem("Low Pass", 1);
    typeSelector.addItem("High Pass", 2);
    typeSelector.addItem("Band Pass", 3);
    typeSelector.setSelectedId(1);
    typeSelector.onChange = [this] { if (onParameterChanged) onParameterChanged(); };
    addAndMakeVisible(typeSelector);

    typeLabel.setText("Type", juce::dontSendNotification);
    typeLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(typeLabel);

    // Filter active button
    filterActiveButton.setButtonText("On");
    filterActiveButton.setToggleState(true, juce::dontSendNotification);
    filterActiveButton.onClick = [this] { if (onParameterChanged) onParameterChanged(); };
    addAndMakeVisible(filterActiveButton);

    filterActiveLabel.setText("Active", juce::dontSendNotification);
    filterActiveLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(filterActiveLabel);

    // Cutoff slider
    setupSlider(cutoffSlider, cutoffLabel, "Cutoff");
    cutoffSlider.setRange(20.0, 20000.0, 1.0);
    cutoffSlider.setValue(1000.0);
    cutoffSlider.setSkewFactorFromMidPoint(1000.0);
    cutoffSlider.setTextValueSuffix(" Hz");

    // Resonance slider
    setupSlider(resonanceSlider, resonanceLabel, "Reso");
    resonanceSlider.setRange(0.1, 10.0, 0.1);
    resonanceSlider.setValue(0.707);
    resonanceSlider.setSkewFactorFromMidPoint(2.0);

    // Env Amount slider
    setupSlider(envAmountSlider, envAmountLabel, "Env Amt");
    envAmountSlider.setRange(-1.0, 1.0, 0.01);
    envAmountSlider.setValue(0.5);
}

void FilterComponent::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text) {
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 56, 14);
    slider.onValueChange = [this] { if (onParameterChanged) onParameterChanged(); };
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void FilterComponent::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    g.setColour(CustomLookAndFeel::backgroundMid.withAlpha(0.5f));
    g.fillRoundedRectangle(bounds, 6.0f);

    g.setColour(CustomLookAndFeel::backgroundLight.withAlpha(0.6f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 6.0f, 1.0f);

    g.setColour(CustomLookAndFeel::accentColor);
    g.setFont(13.0f);
    g.drawText("Filter", 10, 6, 50, 18, juce::Justification::left);
}

void FilterComponent::resized() {
    auto bounds = getLocalBounds().reduced(8);

    // Header row: title (painted) | Active toggle | Type selector
    auto headerRow = bounds.removeFromTop(26);
    headerRow.removeFromLeft(56); // skip "Filter" title

    filterActiveLabel.setBounds(headerRow.removeFromLeft(38));
    headerRow.removeFromLeft(2);
    filterActiveButton.setBounds(headerRow.removeFromLeft(30).reduced(0, 3));
    headerRow.removeFromLeft(6);

    typeLabel.setBounds(headerRow.removeFromLeft(32));
    headerRow.removeFromLeft(2);
    typeSelector.setBounds(headerRow.reduced(0, 2));

    bounds.removeFromTop(4);

    // Three knobs in equal columns
    const int labelH = 16;
    int knobW = bounds.getWidth() / 3;

    auto cutoffArea = bounds.removeFromLeft(knobW);
    cutoffLabel.setBounds(cutoffArea.removeFromTop(labelH));
    cutoffSlider.setBounds(cutoffArea);

    auto resoArea = bounds.removeFromLeft(knobW);
    resonanceLabel.setBounds(resoArea.removeFromTop(labelH));
    resonanceSlider.setBounds(resoArea);

    envAmountLabel.setBounds(bounds.removeFromTop(labelH));
    envAmountSlider.setBounds(bounds);
}

FilterType FilterComponent::getFilterType() const {
    switch (typeSelector.getSelectedId()) {
        case 1: return FilterType::LowPass;
        case 2: return FilterType::HighPass;
        case 3: return FilterType::BandPass;
        default: return FilterType::LowPass;
    }
}

float FilterComponent::getCutoff() const {
    return static_cast<float>(cutoffSlider.getValue());
}

float FilterComponent::getResonance() const {
    return static_cast<float>(resonanceSlider.getValue());
}

float FilterComponent::getEnvAmount() const {
    return static_cast<float>(envAmountSlider.getValue());
}

bool FilterComponent::isFilterActive() const {
    return filterActiveButton.getToggleState();
}

void FilterComponent::setFilterType(FilterType type) {
    int id = 1;
    switch (type) {
        case FilterType::LowPass: id = 1; break;
        case FilterType::HighPass: id = 2; break;
        case FilterType::BandPass: id = 3; break;
    }
    typeSelector.setSelectedId(id, juce::dontSendNotification);
}

void FilterComponent::setCutoff(float freq) {
    cutoffSlider.setValue(freq, juce::dontSendNotification);
}

void FilterComponent::setResonance(float q) {
    resonanceSlider.setValue(q, juce::dontSendNotification);
}

void FilterComponent::setEnvAmount(float amount) {
    envAmountSlider.setValue(amount, juce::dontSendNotification);
}

void FilterComponent::setFilterActive(bool shouldBeActive) {
    filterActiveButton.setToggleState(shouldBeActive, juce::dontSendNotification);
}

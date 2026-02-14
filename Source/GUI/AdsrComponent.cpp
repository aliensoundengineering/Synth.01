#include "AdsrComponent.h"
#include "CustomLookAndFeel.h"

AdsrComponent::AdsrComponent(const juce::String& name)
    : adsrName(name) {

    setupSlider(attackSlider, attackLabel, "A");
    attackSlider.setRange(0.001, 2.0, 0.001);
    attackSlider.setValue(0.1);
    attackSlider.setSkewFactorFromMidPoint(0.3);

    setupSlider(decaySlider, decayLabel, "D");
    decaySlider.setRange(0.001, 2.0, 0.001);
    decaySlider.setValue(0.1);
    decaySlider.setSkewFactorFromMidPoint(0.3);

    setupSlider(sustainSlider, sustainLabel, "S");
    sustainSlider.setRange(0.0, 1.0, 0.01);
    sustainSlider.setValue(0.8);

    setupSlider(releaseSlider, releaseLabel, "R");
    releaseSlider.setRange(0.001, 3.0, 0.001);
    releaseSlider.setValue(0.3);
    releaseSlider.setSkewFactorFromMidPoint(0.5);
}

void AdsrComponent::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text) {
    slider.setSliderStyle(juce::Slider::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 38, 14);
    slider.onValueChange = [this] { if (onParameterChanged) onParameterChanged(); };
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void AdsrComponent::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    g.setColour(CustomLookAndFeel::backgroundMid.withAlpha(0.5f));
    g.fillRoundedRectangle(bounds, 6.0f);

    g.setColour(CustomLookAndFeel::backgroundLight.withAlpha(0.6f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 6.0f, 1.0f);

    g.setColour(CustomLookAndFeel::accentColor);
    g.setFont(13.0f);
    g.drawText(adsrName, 10, 6, getWidth() - 20, 18, juce::Justification::left);
}

void AdsrComponent::resized() {
    auto bounds = getLocalBounds().reduced(8);
    bounds.removeFromTop(26); // title

    const int labelH = 16;
    int sliderW = bounds.getWidth() / 4;

    auto aArea = bounds.removeFromLeft(sliderW);
    attackLabel.setBounds(aArea.removeFromTop(labelH));
    attackSlider.setBounds(aArea.reduced(2, 0));

    auto dArea = bounds.removeFromLeft(sliderW);
    decayLabel.setBounds(dArea.removeFromTop(labelH));
    decaySlider.setBounds(dArea.reduced(2, 0));

    auto sArea = bounds.removeFromLeft(sliderW);
    sustainLabel.setBounds(sArea.removeFromTop(labelH));
    sustainSlider.setBounds(sArea.reduced(2, 0));

    releaseLabel.setBounds(bounds.removeFromTop(labelH));
    releaseSlider.setBounds(bounds.reduced(2, 0));
}

AdsrEnvelope::Parameters AdsrComponent::getParameters() const {
    AdsrEnvelope::Parameters params;
    params.attack = static_cast<float>(attackSlider.getValue());
    params.decay = static_cast<float>(decaySlider.getValue());
    params.sustain = static_cast<float>(sustainSlider.getValue());
    params.release = static_cast<float>(releaseSlider.getValue());
    return params;
}

void AdsrComponent::setParameters(const AdsrEnvelope::Parameters& params) {
    attackSlider.setValue(params.attack, juce::dontSendNotification);
    decaySlider.setValue(params.decay, juce::dontSendNotification);
    sustainSlider.setValue(params.sustain, juce::dontSendNotification);
    releaseSlider.setValue(params.release, juce::dontSendNotification);
}

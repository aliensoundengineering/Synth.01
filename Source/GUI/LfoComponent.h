#pragma once
#include <JuceHeader.h>
#include "../Synth/LfoGenerator.h"

class LfoComponent : public juce::Component {
public:
    LfoComponent();
    ~LfoComponent() override = default;

    void resized() override;
    void paint(juce::Graphics& g) override;

    LfoWaveType getWaveType() const;
    float getRate() const;
    float getDepth() const;
    LfoDestination getDestination() const;

    void setWaveType(LfoWaveType type);
    void setRate(float rate);
    void setDepth(float depth);
    void setDestination(LfoDestination dest);

    std::function<void()> onParameterChanged;

private:
    juce::ComboBox waveSelector;
    juce::ComboBox destinationSelector;
    juce::Slider rateSlider;
    juce::Slider depthSlider;
    
    juce::Label waveLabel;
    juce::Label destLabel;
    juce::Label rateLabel;
    juce::Label depthLabel;

    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LfoComponent)
};

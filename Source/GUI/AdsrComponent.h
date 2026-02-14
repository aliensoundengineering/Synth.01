#pragma once
#include <JuceHeader.h>
#include "../Synth/AdsrEnvelope.h"

class AdsrComponent : public juce::Component {
public:
    AdsrComponent(const juce::String& name);
    ~AdsrComponent() override = default;

    void resized() override;
    void paint(juce::Graphics& g) override;

    AdsrEnvelope::Parameters getParameters() const;
    void setParameters(const AdsrEnvelope::Parameters& params);

    std::function<void()> onParameterChanged;

private:
    juce::String adsrName;
    
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    
    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;

    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AdsrComponent)
};

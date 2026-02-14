#pragma once
#include <JuceHeader.h>
#include "../Synth/Filter.h"

class FilterComponent : public juce::Component {
public:
    FilterComponent();
    ~FilterComponent() override = default;

    void resized() override;
    void paint(juce::Graphics& g) override;

    FilterType getFilterType() const;
    float getCutoff() const;
    float getResonance() const;
    float getEnvAmount() const;

    void setFilterType(FilterType type);
    void setCutoff(float freq);
    void setResonance(float q);
    void setEnvAmount(float amount);

    bool isFilterActive() const;
    void setFilterActive(bool shouldBeActive);

    std::function<void()> onParameterChanged;

private:
    juce::ComboBox typeSelector;
    juce::Slider cutoffSlider;
    juce::Slider resonanceSlider;
    juce::Slider envAmountSlider;
    
    juce::Label typeLabel;
    juce::Label cutoffLabel;
    juce::Label resonanceLabel;
    juce::Label envAmountLabel;

    juce::ToggleButton filterActiveButton;
    juce::Label filterActiveLabel;

    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterComponent)
};

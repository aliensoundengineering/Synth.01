#pragma once
#include <JuceHeader.h>
#include "../Effects/DriveDistortion.h"
#include "../Effects/DelayLine.h"

class EffectsComponent : public juce::Component {
public:
    EffectsComponent();
    ~EffectsComponent() override = default;

    void resized() override;
    void paint(juce::Graphics& g) override;

    // Drive getters
    DriveType getDriveType() const;
    float getDriveAmount() const;
    float getDriveMix() const;
    float getDriveOutputGain() const;

    // Delay getters
    float getDelayTime() const;
    float getDelayFeedback() const;
    float getDelayMix() const;

    // Reverb getters
    float getReverbRoomSize() const;
    float getReverbDamping() const;
    float getReverbWetLevel() const;
    float getReverbDryLevel() const;
    float getReverbWidth() const;

    // Active state getters
    bool isDriveActive() const;
    bool isDelayActive() const;
    bool isReverbActive() const;

    // Active state setters
    void setDriveActive(bool active);
    void setDelayActive(bool active);
    void setReverbActive(bool active);

    // Setters
    void setDriveType(DriveType type);
    void setDriveAmount(float amount);
    void setDriveMix(float mix);
    void setDriveOutputGain(float gain);
    void setDelayTime(float time);
    void setDelayFeedback(float feedback);
    void setDelayMix(float mix);
    void setReverbRoomSize(float size);
    void setReverbDamping(float damping);
    void setReverbWetLevel(float wet);
    void setReverbDryLevel(float dry);
    void setReverbWidth(float width);

    std::function<void()> onParameterChanged;

private:
    // Drive controls
    juce::ComboBox driveTypeSelector;
    juce::Slider driveAmountSlider;
    juce::Slider driveMixSlider;
    juce::Slider driveOutputSlider;
    
    juce::Label driveAmountLabel;
    juce::Label driveMixLabel;
    juce::Label driveOutputLabel;
    
    // Toggle buttons for effect activation
    juce::ToggleButton driveActiveButton { "On" };
    juce::ToggleButton delayActiveButton { "On" };
    juce::ToggleButton reverbActiveButton { "On" };

    // Delay controls
    juce::Slider delayTimeSlider;
    juce::Slider delayFeedbackSlider;
    juce::Slider delayMixSlider;
    
    juce::Label delayTimeLabel;
    juce::Label delayFeedbackLabel;
    juce::Label delayMixLabel;

    // Reverb controls
    juce::Slider reverbRoomSizeSlider;
    juce::Slider reverbDampingSlider;
    juce::Slider reverbWetSlider;
    juce::Slider reverbDrySlider;
    juce::Slider reverbWidthSlider;
    
    juce::Label reverbRoomSizeLabel;
    juce::Label reverbDampingLabel;
    juce::Label reverbWetLabel;
    juce::Label reverbDryLabel;
    juce::Label reverbWidthLabel;

    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectsComponent)
};

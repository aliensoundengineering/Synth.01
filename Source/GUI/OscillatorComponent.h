#pragma once
#include <JuceHeader.h>
#include "../Synth/Oscillator.h"

class OscillatorComponent : public juce::Component {
public:
    OscillatorComponent(const juce::String& name);
    ~OscillatorComponent() override = default;

    void resized() override;
    void paint(juce::Graphics& g) override;

    // Getters for parameter values
    WaveType getWaveType() const;
    float getGain() const;
    float getDetune() const;
    int getOctave() const;

    // Setters
    void setWaveType(WaveType type);
    void setGain(float gain);
    void setDetune(float cents);
    void setOctave(int octave);

    // Callback when parameters change
    std::function<void()> onParameterChanged;

private:
    juce::String oscName;
    
    juce::ComboBox waveSelector;
    juce::Slider gainSlider;
    juce::Slider detuneSlider;
    juce::Slider octaveSlider;
    
    juce::Label waveLabel;
    juce::Label gainLabel;
    juce::Label detuneLabel;
    juce::Label octaveLabel;

    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorComponent)
};

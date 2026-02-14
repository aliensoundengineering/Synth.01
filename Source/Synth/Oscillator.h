#pragma once
#include <JuceHeader.h>

enum class WaveType {
    Sine,
    Saw,
    Square
};

class Oscillator {
public:
    Oscillator();
    ~Oscillator() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    
    void setWaveType(WaveType type);
    void setFrequency(float frequency);
    void setDetune(float cents);
    void setGain(float gain);
    
    float getNextSample();
    
    WaveType getWaveType() const { return waveType; }
    float getGain() const { return gain; }
    float getDetune() const { return detuneCents; }

private:
    void updateFrequency();
    
    WaveType waveType = WaveType::Saw;
    float baseFrequency = 440.0f;
    float detuneCents = 0.0f;
    float gain = 1.0f;
    float phase = 0.0f;
    float phaseIncrement = 0.0f;
    double sampleRate = 44100.0;
    
    // For anti-aliased waveforms
    float polyBlep(float t, float dt);
};

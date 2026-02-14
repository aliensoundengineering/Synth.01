#pragma once
#include <JuceHeader.h>

enum class LfoWaveType {
    Sine,
    Triangle,
    Square,
    SawUp,
    SawDown,
    Random
};

enum class LfoDestination {
    None,
    FilterCutoff,
    Osc1Pitch,
    Osc2Pitch,
    Osc1Gain,
    Osc2Gain
};

class LfoGenerator {
public:
    LfoGenerator();
    ~LfoGenerator() = default;

    void prepare(double sampleRate);
    void reset();
    
    void setRate(float hz);
    void setDepth(float depth);  // 0-1
    void setWaveType(LfoWaveType type);
    void setDestination(LfoDestination dest);
    
    float getRate() const { return rate; }
    float getDepth() const { return depth; }
    LfoWaveType getWaveType() const { return waveType; }
    LfoDestination getDestination() const { return destination; }
    
    float getNextSample();
    float getCurrentValue() const { return currentValue; }

private:
    LfoWaveType waveType = LfoWaveType::Sine;
    LfoDestination destination = LfoDestination::FilterCutoff;
    float rate = 1.0f;      // Hz
    float depth = 0.5f;     // 0-1
    float phase = 0.0f;
    float phaseIncrement = 0.0f;
    float currentValue = 0.0f;
    double sampleRate = 44100.0;
    
    // For random/S&H
    float randomValue = 0.0f;
    juce::Random random;
};

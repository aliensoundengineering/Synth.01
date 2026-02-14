#include "LfoGenerator.h"

LfoGenerator::LfoGenerator() = default;

void LfoGenerator::prepare(double sr) {
    sampleRate = sr;
    phaseIncrement = static_cast<float>(rate / sampleRate);
    reset();
}

void LfoGenerator::reset() {
    phase = 0.0f;
    currentValue = 0.0f;
    randomValue = random.nextFloat() * 2.0f - 1.0f;
}

void LfoGenerator::setRate(float hz) {
    rate = juce::jlimit(0.01f, 20.0f, hz);
    phaseIncrement = static_cast<float>(rate / sampleRate);
}

void LfoGenerator::setDepth(float d) {
    depth = juce::jlimit(0.0f, 1.0f, d);
}

void LfoGenerator::setWaveType(LfoWaveType type) {
    waveType = type;
}

void LfoGenerator::setDestination(LfoDestination dest) {
    destination = dest;
}

float LfoGenerator::getNextSample() {
    float sample = 0.0f;
    
    switch (waveType) {
        case LfoWaveType::Sine:
            sample = std::sin(phase * juce::MathConstants<float>::twoPi);
            break;
            
        case LfoWaveType::Triangle:
            if (phase < 0.25f)
                sample = phase * 4.0f;
            else if (phase < 0.75f)
                sample = 1.0f - (phase - 0.25f) * 4.0f;
            else
                sample = -1.0f + (phase - 0.75f) * 4.0f;
            break;
            
        case LfoWaveType::Square:
            sample = phase < 0.5f ? 1.0f : -1.0f;
            break;
            
        case LfoWaveType::SawUp:
            sample = 2.0f * phase - 1.0f;
            break;
            
        case LfoWaveType::SawDown:
            sample = 1.0f - 2.0f * phase;
            break;
            
        case LfoWaveType::Random:
            // Sample and hold - update on phase wrap
            sample = randomValue;
            break;
    }
    
    // Increment phase
    float oldPhase = phase;
    phase += phaseIncrement;
    if (phase >= 1.0f) {
        phase -= 1.0f;
        // Update random value on cycle complete
        if (waveType == LfoWaveType::Random) {
            randomValue = random.nextFloat() * 2.0f - 1.0f;
        }
    }
    
    currentValue = sample * depth;
    return currentValue;
}

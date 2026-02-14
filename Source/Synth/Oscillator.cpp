#include "Oscillator.h"

Oscillator::Oscillator() = default;

void Oscillator::prepare(const juce::dsp::ProcessSpec& spec) {
    sampleRate = spec.sampleRate;
    reset();
}

void Oscillator::reset() {
    phase = 0.0f;
}

void Oscillator::setWaveType(WaveType type) {
    waveType = type;
}

void Oscillator::setFrequency(float frequency) {
    baseFrequency = frequency;
    updateFrequency();
}

void Oscillator::setDetune(float cents) {
    detuneCents = cents;
    updateFrequency();
}

void Oscillator::setGain(float g) {
    gain = g;
}

void Oscillator::updateFrequency() {
    // Convert cents to frequency ratio: ratio = 2^(cents/1200)
    float detuneRatio = std::pow(2.0f, detuneCents / 1200.0f);
    float actualFrequency = baseFrequency * detuneRatio;
    phaseIncrement = static_cast<float>(actualFrequency / sampleRate);
}

// PolyBLEP (polynomial bandlimited step) for anti-aliasing
float Oscillator::polyBlep(float t, float dt) {
    if (t < dt) {
        t /= dt;
        return t + t - t * t - 1.0f;
    }
    else if (t > 1.0f - dt) {
        t = (t - 1.0f) / dt;
        return t * t + t + t + 1.0f;
    }
    return 0.0f;
}

float Oscillator::getNextSample() {
    float sample = 0.0f;
    
    switch (waveType) {
        case WaveType::Sine:
            sample = std::sin(phase * juce::MathConstants<float>::twoPi);
            break;
            
        case WaveType::Saw: {
            // Naive saw: goes from -1 to 1
            sample = 2.0f * phase - 1.0f;
            // Apply PolyBLEP anti-aliasing
            sample -= polyBlep(phase, phaseIncrement);
            break;
        }
            
        case WaveType::Square: {
            // Naive square
            sample = phase < 0.5f ? 1.0f : -1.0f;
            // Apply PolyBLEP at transitions
            sample += polyBlep(phase, phaseIncrement);
            sample -= polyBlep(std::fmod(phase + 0.5f, 1.0f), phaseIncrement);
            break;
        }
    }
    
    // Increment phase
    phase += phaseIncrement;
    if (phase >= 1.0f)
        phase -= 1.0f;
    
    return sample * gain;
}

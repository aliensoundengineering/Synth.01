#include "Filter.h"

Filter::Filter() = default;

void Filter::prepare(const juce::dsp::ProcessSpec& spec) {
    sampleRate = spec.sampleRate;
    reset();
    updateCoefficients(baseCutoff);
}

void Filter::reset() {
    ic1eq = 0.0f;
    ic2eq = 0.0f;
}

void Filter::setType(FilterType type) {
    filterType = type;
}

void Filter::setCutoff(float frequency) {
    baseCutoff = juce::jlimit(20.0f, 20000.0f, frequency);
}

void Filter::setResonance(float q) {
    resonance = juce::jlimit(0.1f, 10.0f, q);
}

void Filter::setEnvelopeAmount(float amount) {
    envAmount = juce::jlimit(-1.0f, 1.0f, amount);
}

void Filter::updateCoefficients(float modulatedCutoff) {
    // Clamp the modulated cutoff frequency
    modulatedCutoff = juce::jlimit(20.0f, static_cast<float>(sampleRate * 0.49), modulatedCutoff);
    
    // State Variable Filter (SVF) coefficients
    // Based on Andy Cytomic's technical paper
    g = std::tan(juce::MathConstants<float>::pi * modulatedCutoff / static_cast<float>(sampleRate));
    k = 1.0f / resonance;
    a1 = 1.0f / (1.0f + g * (g + k));
    a2 = g * a1;
    a3 = g * a2;
}

float Filter::processSample(float sample, float envValue, float lfoValue) {
    // Calculate modulated cutoff
    // Envelope modulation: exponential mapping for musical response
    float envMod = envAmount * envValue * 4.0f;  // 4 octaves max range
    float lfoMod = lfoValue * 2.0f;  // 2 octaves range for LFO
    
    float modulatedCutoff = baseCutoff * std::pow(2.0f, envMod + lfoMod);
    
    // Update coefficients for new cutoff
    updateCoefficients(modulatedCutoff);
    
    // SVF processing
    float v3 = sample - ic2eq;
    float v1 = a1 * ic1eq + a2 * v3;
    float v2 = ic2eq + a2 * ic1eq + a3 * v3;
    
    ic1eq = 2.0f * v1 - ic1eq;
    ic2eq = 2.0f * v2 - ic2eq;
    
    // Select output based on filter type
    switch (filterType) {
        case FilterType::LowPass:
            return v2;
        case FilterType::HighPass:
            return sample - k * v1 - v2;
        case FilterType::BandPass:
            return v1;
        default:
            return v2;
    }
}

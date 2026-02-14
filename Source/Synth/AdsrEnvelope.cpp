#include "AdsrEnvelope.h"

AdsrEnvelope::AdsrEnvelope() = default;

void AdsrEnvelope::prepare(double sr) {
    sampleRate = sr;
    calculateRates();
    reset();
}

void AdsrEnvelope::reset() {
    currentState = State::Idle;
    currentLevel = 0.0f;
}

void AdsrEnvelope::setParameters(const Parameters& params) {
    parameters = params;
    calculateRates();
}

void AdsrEnvelope::calculateRates() {
    // Calculate rate per sample for each stage
    // Rate = 1 / (time_in_seconds * sample_rate)
    float minTime = 0.001f; // 1ms minimum to avoid division issues
    
    attackRate = 1.0f / (std::max(parameters.attack, minTime) * static_cast<float>(sampleRate));
    decayRate = 1.0f / (std::max(parameters.decay, minTime) * static_cast<float>(sampleRate));
    releaseRate = 1.0f / (std::max(parameters.release, minTime) * static_cast<float>(sampleRate));
}

void AdsrEnvelope::noteOn() {
    currentState = State::Attack;
}

void AdsrEnvelope::noteOff() {
    if (currentState != State::Idle)
        currentState = State::Release;
}

float AdsrEnvelope::getNextSample() {
    switch (currentState) {
        case State::Idle:
            currentLevel = 0.0f;
            break;
            
        case State::Attack:
            currentLevel += attackRate;
            if (currentLevel >= 1.0f) {
                currentLevel = 1.0f;
                currentState = State::Decay;
            }
            break;
            
        case State::Decay:
            currentLevel -= decayRate * (1.0f - parameters.sustain);
            if (currentLevel <= parameters.sustain) {
                currentLevel = parameters.sustain;
                currentState = State::Sustain;
            }
            break;
            
        case State::Sustain:
            currentLevel = parameters.sustain;
            break;
            
        case State::Release:
            currentLevel -= releaseRate * currentLevel;
            if (currentLevel <= 0.001f) {
                currentLevel = 0.0f;
                currentState = State::Idle;
            }
            break;
    }
    
    return currentLevel;
}

bool AdsrEnvelope::isActive() const {
    return currentState != State::Idle;
}

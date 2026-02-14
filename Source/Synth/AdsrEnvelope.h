#pragma once
#include <JuceHeader.h>

// Custom ADSR envelope with additional features
class AdsrEnvelope {
public:
    struct Parameters {
        float attack = 0.1f;   // seconds
        float decay = 0.1f;    // seconds
        float sustain = 0.8f;  // level 0-1
        float release = 0.3f;  // seconds
    };

    AdsrEnvelope();
    ~AdsrEnvelope() = default;

    void prepare(double sampleRate);
    void reset();
    
    void setParameters(const Parameters& params);
    Parameters getParameters() const { return parameters; }
    
    void noteOn();
    void noteOff();
    
    float getNextSample();
    bool isActive() const;

private:
    enum class State {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };

    void calculateRates();
    
    Parameters parameters;
    State currentState = State::Idle;
    float currentLevel = 0.0f;
    float attackRate = 0.0f;
    float decayRate = 0.0f;
    float releaseRate = 0.0f;
    double sampleRate = 44100.0;
};

#pragma once
#include "Oscillator.h"
#include "AdsrEnvelope.h"
#include "LfoGenerator.h"
#include "Filter.h"

// Parameters that can be shared across all voices
struct SynthParameters {
    // Oscillator 1
    WaveType osc1Wave = WaveType::Saw;
    float osc1Gain = 0.5f;
    float osc1Detune = 0.0f;
    int osc1Octave = 0;
    
    // Oscillator 2
    WaveType osc2Wave = WaveType::Square;
    float osc2Gain = 0.5f;
    float osc2Detune = 0.0f;
    int osc2Octave = 0;
    
    // Filter
    FilterType filterType = FilterType::LowPass;
    float filterCutoff = 1000.0f;
    float filterResonance = 0.707f;
    float filterEnvAmount = 0.5f;
    bool filterActive = true; 
    
    // Amp ADSR
    AdsrEnvelope::Parameters ampAdsr = { 0.01f, 0.1f, 0.8f, 0.3f };
    
    // Filter ADSR
    AdsrEnvelope::Parameters filterAdsr = { 0.01f, 0.2f, 0.5f, 0.3f };
    
    // LFO
    LfoWaveType lfoWave = LfoWaveType::Sine;
    float lfoRate = 1.0f;
    float lfoDepth = 0.5f;
    LfoDestination lfoDestination = LfoDestination::FilterCutoff;
    
    // Master
    float masterGain = 0.7f;
};

class SynthVoice : public juce::SynthesiserVoice {
public:
    SynthVoice();
    ~SynthVoice() override = default;

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newValue) override;
    void controllerMoved(int controllerNumber, int newValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void setParameters(const SynthParameters& params);
    
    // Access to LFO for global modulation
    LfoGenerator& getLfo() { return lfo; }

private:
    float getFrequencyForNote(int midiNote, int octaveOffset, float detuneCents);
    
    Oscillator osc1;
    Oscillator osc2;
    Filter filter;
    AdsrEnvelope ampEnvelope;
    AdsrEnvelope filterEnvelope;
    LfoGenerator lfo;
    
    SynthParameters params;
    
    float currentVelocity = 0.0f;
    int currentNote = 0;
    float pitchWheelValue = 0.0f;
    
    bool isPrepared = false;
};

#pragma once
#include <JuceHeader.h>
#include "Synth/SynthSound.h"
#include "Synth/SynthVoice.h"
#include "Effects/DriveDistortion.h"
#include "Effects/DelayLine.h"

class SynthAudioSource : public juce::AudioSource {
public:
    SynthAudioSource(juce::MidiKeyboardState& keyState);
    ~SynthAudioSource() override = default;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void setParameters(const SynthParameters& params);
    SynthParameters getParameters() const { return currentParams; }
    
    void setDriveParameters(DriveType type, float amount, float mix, float output);
    void setDelayParameters(float time, float feedback, float mix);
    void setReverbParameters(float roomSize, float damping, float wetLevel, float dryLevel, float width);
    
    DriveDistortion& getDrive() { return drive; }
    DelayLine& getDelay() { return delay; }
    juce::Reverb& getReverb() { return reverb; }
    
    juce::Synthesiser& getSynth() { return synth; }

private:
    juce::MidiKeyboardState& keyboardState;
    juce::Synthesiser synth;
    
    DriveDistortion drive;
    DelayLine delay;
    juce::Reverb reverb;
    
    SynthParameters currentParams;
    
    static constexpr int numVoices = 8;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthAudioSource)
};

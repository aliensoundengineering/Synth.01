#pragma once
#include <JuceHeader.h>
#include "Synth/SynthSound.h"
#include "Synth/SynthVoice.h"
#include "Effects/DriveDistortion.h"
#include "Effects/DelayLine.h"
#include <mutex>
#include <atomic>

class CodingSoundsSynthProcessor : public juce::AudioProcessor {
public:
    CodingSoundsSynthProcessor();
    ~CodingSoundsSynthProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Synth parameter methods
    void setParameters(const SynthParameters& params);
    SynthParameters getParameters() const { return currentParams; }
    
    void setDriveParameters(DriveType type, float amount, float mix, float output);
    void setDelayParameters(float time, float feedback, float mix);
    void setReverbParameters(float roomSize, float damping, float wetLevel, float dryLevel, float width);

    void setDriveActive(bool active);
    void setDelayActive(bool active);
    void setReverbActive(bool active);
    
    juce::MidiKeyboardState& getKeyboardState() { return keyboardState; }
    juce::Synthesiser& getSynth() { return synth; }

private:
    juce::MidiKeyboardState keyboardState;
    juce::Synthesiser synth;
    
    DriveDistortion drive;
    DelayLine delay;
    juce::Reverb reverb;
    
    SynthParameters currentParams;
    
    // Thread-safe parameter updates
    std::mutex paramMutex;
    std::atomic<bool> paramsNeedUpdate { false };
    SynthParameters pendingParams;
    
    // Effect parameter flags
    std::atomic<bool> driveParamsNeedUpdate { false };
    std::atomic<bool> delayParamsNeedUpdate { false };
    std::atomic<bool> reverbParamsNeedUpdate { false };
    
    // Pending effect parameters
    struct DriveParams {
        DriveType type = DriveType::Soft;
        float amount = 0.0f;
        float mix = 0.0f;
        float output = 1.0f;
    };
    DriveParams pendingDriveParams;
    
    struct DelayParams {
        float time = 0.0f;
        float feedback = 0.0f;
        float mix = 0.0f;
    };
    DelayParams pendingDelayParams;
    
    struct ReverbParams {
        float roomSize = 0.5f;
        float damping = 0.5f;
        float wetLevel = 0.33f;
        float dryLevel = 0.4f;
        float width = 1.0f;
    };
    ReverbParams pendingReverbParams;
    
    // Effect active flags (atomic for thread-safe access from audio thread)
    std::atomic<bool> driveActive { true };
    std::atomic<bool> delayActive { true };
    std::atomic<bool> reverbActive { true };

    static constexpr int numVoices = 8;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CodingSoundsSynthProcessor)
};

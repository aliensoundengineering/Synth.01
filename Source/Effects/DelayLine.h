#pragma once
#include <JuceHeader.h>

class DelayLine {
public:
    DelayLine();
    ~DelayLine() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    
    void setDelayTime(float timeMs);    // milliseconds
    void setFeedback(float feedback);   // 0-0.95
    void setMix(float mix);             // 0-1 (dry/wet)
    void setSync(bool sync);            // sync to tempo
    void setTempoSync(float bpm, float noteFraction);  // 1/4, 1/8, etc.
    
    float getDelayTime() const { return delayTimeMs; }
    float getFeedback() const { return feedbackAmount; }
    float getMix() const { return mixAmount; }
    bool isSynced() const { return tempoSync; }
    
    void process(juce::AudioBuffer<float>& buffer);
    float processSample(float sample, int channel);

private:
    static constexpr int maxDelaySamples = 192000;  // 2 seconds at 96kHz
    
    std::array<std::vector<float>, 2> delayBuffer;
    std::array<int, 2> writePosition = { 0, 0 };
    
    float delayTimeMs = 250.0f;
    float feedbackAmount = 0.4f;
    float mixAmount = 0.3f;
    bool tempoSync = false;
    
    int delaySamples = 0;
    double sampleRate = 44100.0;
    
    // Smoothing for delay time changes
    juce::SmoothedValue<float> smoothedDelaySamples;
};

#include "DelayLine.h"

DelayLine::DelayLine() = default;

void DelayLine::prepare(const juce::dsp::ProcessSpec& spec) {
    sampleRate = spec.sampleRate;
    
    // Resize delay buffers
    for (auto& buffer : delayBuffer) {
        buffer.resize(maxDelaySamples, 0.0f);
    }
    
    smoothedDelaySamples.reset(sampleRate, 0.05);  // 50ms smoothing
    
    reset();
    setDelayTime(delayTimeMs);
}

void DelayLine::reset() {
    for (auto& buffer : delayBuffer) {
        std::fill(buffer.begin(), buffer.end(), 0.0f);
    }
    writePosition = { 0, 0 };
}

void DelayLine::setDelayTime(float timeMs) {
    delayTimeMs = juce::jlimit(1.0f, 2000.0f, timeMs);
    delaySamples = static_cast<int>((delayTimeMs / 1000.0f) * sampleRate);
    delaySamples = std::min(delaySamples, maxDelaySamples - 1);
    smoothedDelaySamples.setTargetValue(static_cast<float>(delaySamples));
}

void DelayLine::setFeedback(float feedback) {
    feedbackAmount = juce::jlimit(0.0f, 0.95f, feedback);
}

void DelayLine::setMix(float mix) {
    mixAmount = juce::jlimit(0.0f, 1.0f, mix);
}

void DelayLine::setSync(bool sync) {
    tempoSync = sync;
}

void DelayLine::setTempoSync(float bpm, float noteFraction) {
    if (tempoSync && bpm > 0.0f) {
        // Calculate delay time based on tempo and note fraction
        // noteFraction: 1.0 = whole note, 0.5 = half, 0.25 = quarter, etc.
        float beatsPerSecond = bpm / 60.0f;
        float delayMs = (1000.0f / beatsPerSecond) * (noteFraction * 4.0f);
        setDelayTime(delayMs);
    }
}

float DelayLine::processSample(float sample, int channel) {
    if (channel >= 2) return sample;
    
    auto& buffer = delayBuffer[channel];
    auto& writePos = writePosition[channel];
    
    // Get current smoothed delay
    float currentDelay = smoothedDelaySamples.getNextValue();
    int intDelay = static_cast<int>(currentDelay);
    float fracDelay = currentDelay - intDelay;
    
    // Calculate read positions for linear interpolation
    int readPos1 = writePos - intDelay;
    int readPos2 = readPos1 - 1;
    
    // Wrap around
    if (readPos1 < 0) readPos1 += maxDelaySamples;
    if (readPos2 < 0) readPos2 += maxDelaySamples;
    
    // Linear interpolation for smooth delay time changes
    float delayedSample1 = buffer[readPos1];
    float delayedSample2 = buffer[readPos2];
    float delayedSample = delayedSample1 + fracDelay * (delayedSample2 - delayedSample1);
    
    // Write to buffer (input + feedback)
    buffer[writePos] = sample + delayedSample * feedbackAmount;
    
    // Increment write position
    writePos++;
    if (writePos >= maxDelaySamples)
        writePos = 0;
    
    // Mix dry and wet
    return sample * (1.0f - mixAmount) + delayedSample * mixAmount;
}

void DelayLine::process(juce::AudioBuffer<float>& buffer) {
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            channelData[sample] = processSample(channelData[sample], channel);
        }
    }
}

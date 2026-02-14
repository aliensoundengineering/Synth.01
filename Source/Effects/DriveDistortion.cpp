#include "DriveDistortion.h"

DriveDistortion::DriveDistortion() = default;

void DriveDistortion::prepare(const juce::dsp::ProcessSpec& spec) {
    sampleRate = spec.sampleRate;
    reset();
}

void DriveDistortion::reset() {
    dcBlockerX1 = 0.0f;
    dcBlockerY1 = 0.0f;
}

void DriveDistortion::setDrive(float amount) {
    driveAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void DriveDistortion::setMix(float mix) {
    mixAmount = juce::jlimit(0.0f, 1.0f, mix);
}

void DriveDistortion::setType(DriveType type) {
    driveType = type;
}

void DriveDistortion::setOutputGain(float gain) {
    outputGain = juce::jlimit(0.0f, 1.0f, gain);
}

float DriveDistortion::softClip(float sample) {
    // Soft clipping using tanh
    float drive = 1.0f + driveAmount * 20.0f;
    return std::tanh(sample * drive) / std::tanh(drive);
}

float DriveDistortion::hardClip(float sample) {
    // Hard clipping with adjustable threshold
    float threshold = 1.0f - driveAmount * 0.9f;
    float drive = 1.0f + driveAmount * 10.0f;
    sample *= drive;
    
    if (sample > threshold)
        return threshold;
    else if (sample < -threshold)
        return -threshold;
    return sample;
}

float DriveDistortion::tubeClip(float sample) {
    // Asymmetric tube-like saturation
    float drive = 1.0f + driveAmount * 15.0f;
    sample *= drive;
    
    if (sample >= 0.0f) {
        // Positive side - softer
        return 1.0f - std::exp(-sample);
    } else {
        // Negative side - slightly harder
        return -1.0f + std::exp(sample);
    }
}

float DriveDistortion::fuzzClip(float sample) {
    // Aggressive fuzz with asymmetric clipping
    float drive = 1.0f + driveAmount * 30.0f;
    sample *= drive;
    
    float sign = sample >= 0.0f ? 1.0f : -1.0f;
    float absVal = std::abs(sample);
    
    // Polynomial waveshaping for fuzz character
    float shaped = absVal - 0.25f * absVal * absVal;
    if (shaped < 0.0f) shaped = 0.0f;
    
    return sign * std::min(shaped, 1.0f);
}

float DriveDistortion::processSample(float sample) {
    if (driveAmount < 0.001f)
        return sample * outputGain;
    
    float drySample = sample;
    float wetSample = 0.0f;
    
    switch (driveType) {
        case DriveType::Soft:
            wetSample = softClip(sample);
            break;
        case DriveType::Hard:
            wetSample = hardClip(sample);
            break;
        case DriveType::Tube:
            wetSample = tubeClip(sample);
            break;
        case DriveType::Fuzz:
            wetSample = fuzzClip(sample);
            break;
    }
    
    // DC blocking filter
    float output = wetSample - dcBlockerX1 + dcBlockerCoeff * dcBlockerY1;
    dcBlockerX1 = wetSample;
    dcBlockerY1 = output;
    wetSample = output;
    
    // Mix dry/wet and apply output gain
    return (drySample * (1.0f - mixAmount) + wetSample * mixAmount) * outputGain;
}

void DriveDistortion::process(juce::AudioBuffer<float>& buffer) {
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            channelData[sample] = processSample(channelData[sample]);
        }
    }
}

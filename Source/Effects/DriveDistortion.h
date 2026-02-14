#pragma once
#include <JuceHeader.h>

enum class DriveType {
    Soft,      // Soft clipping (tanh)
    Hard,      // Hard clipping
    Tube,      // Tube-like saturation
    Fuzz       // Aggressive fuzz
};

class DriveDistortion {
public:
    DriveDistortion();
    ~DriveDistortion() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    
    void setDrive(float amount);    // 0-1
    void setMix(float mix);         // 0-1 (dry/wet)
    void setType(DriveType type);
    void setOutputGain(float gain); // 0-1
    
    float getDrive() const { return driveAmount; }
    float getMix() const { return mixAmount; }
    DriveType getType() const { return driveType; }
    float getOutputGain() const { return outputGain; }
    
    void process(juce::AudioBuffer<float>& buffer);
    float processSample(float sample);

private:
    float softClip(float sample);
    float hardClip(float sample);
    float tubeClip(float sample);
    float fuzzClip(float sample);
    
    DriveType driveType = DriveType::Soft;
    float driveAmount = 0.0f;
    float mixAmount = 1.0f;
    float outputGain = 0.7f;
    double sampleRate = 44100.0;
    
    // DC blocker
    float dcBlockerX1 = 0.0f;
    float dcBlockerY1 = 0.0f;
    static constexpr float dcBlockerCoeff = 0.995f;
};

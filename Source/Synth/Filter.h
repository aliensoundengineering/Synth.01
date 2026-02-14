#pragma once
#include <JuceHeader.h>

enum class FilterType {
    LowPass,
    HighPass,
    BandPass
};

class Filter {
public:
    Filter();
    ~Filter() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    
    void setType(FilterType type);
    void setCutoff(float frequency);
    void setResonance(float q);
    void setEnvelopeAmount(float amount);  // -1 to 1
    
    float getCutoff() const { return baseCutoff; }
    float getResonance() const { return resonance; }
    float getEnvelopeAmount() const { return envAmount; }
    FilterType getType() const { return filterType; }
    
    // Process single sample with envelope modulation
    float processSample(float sample, float envValue, float lfoValue = 0.0f);
    
private:
    void updateCoefficients(float modulatedCutoff);
    
    FilterType filterType = FilterType::LowPass;
    float baseCutoff = 1000.0f;
    float resonance = 0.707f;
    float envAmount = 0.0f;
    double sampleRate = 44100.0;
    
    // State variable filter coefficients
    float ic1eq = 0.0f;
    float ic2eq = 0.0f;
    
    // Cached coefficients
    float g = 0.0f;
    float k = 0.0f;
    float a1 = 0.0f;
    float a2 = 0.0f;
    float a3 = 0.0f;
};

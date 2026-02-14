#pragma once
#include <JuceHeader.h>

// Simple SynthSound class that accepts all MIDI channels and notes
class SynthSound : public juce::SynthesiserSound {
public:
    SynthSound() = default;
    ~SynthSound() override = default;

    bool appliesToNote(int midiNoteNumber) override { return true; }
    bool appliesToChannel(int midiChannel) override { return true; }
};

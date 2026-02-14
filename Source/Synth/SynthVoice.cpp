#include "SynthVoice.h"
#include "SynthSound.h"

SynthVoice::SynthVoice() = default;

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound) {
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels) {
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(numChannels);
    
    osc1.prepare(spec);
    osc2.prepare(spec);
    filter.prepare(spec);
    ampEnvelope.prepare(sampleRate);
    filterEnvelope.prepare(sampleRate);
    lfo.prepare(sampleRate);
    
    isPrepared = true;
}

void SynthVoice::setParameters(const SynthParameters& newParams) {
    params = newParams;
    
    // Update oscillators
    osc1.setWaveType(params.osc1Wave);
    osc1.setGain(params.osc1Gain);
    osc1.setDetune(params.osc1Detune);
    
    osc2.setWaveType(params.osc2Wave);
    osc2.setGain(params.osc2Gain);
    osc2.setDetune(params.osc2Detune);
    
    // Update filter
    filter.setType(params.filterType);
    filter.setCutoff(params.filterCutoff);
    filter.setResonance(params.filterResonance);
    filter.setEnvelopeAmount(params.filterEnvAmount);
    
    // Update envelopes
    ampEnvelope.setParameters(params.ampAdsr);
    filterEnvelope.setParameters(params.filterAdsr);
    
    // Update LFO
    lfo.setWaveType(params.lfoWave);
    lfo.setRate(params.lfoRate);
    lfo.setDepth(params.lfoDepth);
    lfo.setDestination(params.lfoDestination);
}

float SynthVoice::getFrequencyForNote(int midiNote, int octaveOffset, float detuneCents) {
    // Apply octave offset and pitch wheel
    float noteWithOffset = static_cast<float>(midiNote + octaveOffset * 12) + pitchWheelValue * 2.0f;
    return 440.0f * std::pow(2.0f, (noteWithOffset - 69.0f) / 12.0f);
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) {
    currentNote = midiNoteNumber;
    currentVelocity = velocity;
    pitchWheelValue = (currentPitchWheelPosition - 8192) / 8192.0f;
    
    // Set oscillator frequencies
    float freq1 = getFrequencyForNote(midiNoteNumber, params.osc1Octave, params.osc1Detune);
    float freq2 = getFrequencyForNote(midiNoteNumber, params.osc2Octave, params.osc2Detune);
    
    osc1.setFrequency(freq1);
    osc2.setFrequency(freq2);
    
    // Reset oscillator phases for consistent attack
    osc1.reset();
    osc2.reset();
    
    // Start envelopes
    ampEnvelope.noteOn();
    filterEnvelope.noteOn();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff) {
    if (allowTailOff) {
        ampEnvelope.noteOff();
        filterEnvelope.noteOff();
    } else {
        clearCurrentNote();
        ampEnvelope.reset();
        filterEnvelope.reset();
    }
}

void SynthVoice::pitchWheelMoved(int newValue) {
    pitchWheelValue = (newValue - 8192) / 8192.0f;
    
    if (currentNote > 0) {
        float freq1 = getFrequencyForNote(currentNote, params.osc1Octave, params.osc1Detune);
        float freq2 = getFrequencyForNote(currentNote, params.osc2Octave, params.osc2Detune);
        
        osc1.setFrequency(freq1);
        osc2.setFrequency(freq2);
    }
}

void SynthVoice::controllerMoved(int controllerNumber, int newValue) {
    // Handle MIDI CC messages here if needed
    // For example, CC1 (mod wheel) could control LFO depth
    if (controllerNumber == 1) {
        float normalizedValue = newValue / 127.0f;
        lfo.setDepth(normalizedValue);
    }
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) {
    if (!isPrepared || !isVoiceActive())
        return;
    
    for (int sample = 0; sample < numSamples; ++sample) {
        // Get LFO value
        float lfoValue = lfo.getNextSample();
        
        // Apply LFO to pitch if destination is set
        if (params.lfoDestination == LfoDestination::Osc1Pitch || 
            params.lfoDestination == LfoDestination::Osc2Pitch) {
            
            float pitchMod = lfoValue * 100.0f;  // Max 100 cents modulation
            
            if (params.lfoDestination == LfoDestination::Osc1Pitch) {
                osc1.setDetune(params.osc1Detune + pitchMod);
            } else {
                osc2.setDetune(params.osc2Detune + pitchMod);
            }
            
            // Update frequencies
            float freq1 = getFrequencyForNote(currentNote, params.osc1Octave, params.osc1Detune);
            float freq2 = getFrequencyForNote(currentNote, params.osc2Octave, params.osc2Detune);
            osc1.setFrequency(freq1);
            osc2.setFrequency(freq2);
        }
        
        // Generate oscillator output
        float osc1Sample = osc1.getNextSample();
        float osc2Sample = osc2.getNextSample();
        
        // Apply LFO to gain if destination is set
        float gain1Mod = 1.0f;
        float gain2Mod = 1.0f;
        
        if (params.lfoDestination == LfoDestination::Osc1Gain) {
            gain1Mod = 1.0f + lfoValue * 0.5f;
        } else if (params.lfoDestination == LfoDestination::Osc2Gain) {
            gain2Mod = 1.0f + lfoValue * 0.5f;
        }
        
        // Mix oscillators
        float mixedSample = (osc1Sample * gain1Mod + osc2Sample * gain2Mod);
        
        // Get envelope values
        float filterEnvValue = filterEnvelope.getNextSample();
        float ampEnvValue = ampEnvelope.getNextSample();
        
        // Apply LFO to filter if destination is set
        float filterLfoMod = (params.lfoDestination == LfoDestination::FilterCutoff) ? lfoValue : 0.0f;
        
        // Process through filter only if active
        float filteredSample = mixedSample;
        if (params.filterActive)
            filteredSample = filter.processSample(mixedSample, filterEnvValue, filterLfoMod);
        
        // Apply amplitude envelope and velocity
        float outputSample = filteredSample * ampEnvValue * currentVelocity * params.masterGain;
        
        // Add to output buffer (all channels)
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
            outputBuffer.addSample(channel, startSample + sample, outputSample);
        }
        
        // Check if voice should stop
        if (!ampEnvelope.isActive()) {
            clearCurrentNote();
            break;
        }
    }
}

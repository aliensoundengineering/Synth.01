#include "SynthAudioSource.h"

SynthAudioSource::SynthAudioSource(juce::MidiKeyboardState& keyState)
    : keyboardState(keyState) {
    
    // Add voices to synthesizer
    for (int i = 0; i < numVoices; ++i) {
        synth.addVoice(new SynthVoice());
    }
    
    // Add sound
    synth.addSound(new SynthSound());
}

void SynthAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    // Prepare all voices
    for (int i = 0; i < synth.getNumVoices(); ++i) {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            voice->prepareToPlay(sampleRate, samplesPerBlockExpected, 2);
        }
    }
    
    // Prepare effects
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlockExpected);
    spec.numChannels = 2;
    
    drive.prepare(spec);
    delay.prepare(spec);
    reverb.setSampleRate(sampleRate);
}

void SynthAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    bufferToFill.clearActiveBufferRegion();
    
    // Get MIDI messages from keyboard state
    juce::MidiBuffer midiBuffer;
    keyboardState.processNextMidiBuffer(midiBuffer, bufferToFill.startSample, 
                                         bufferToFill.numSamples, true);
    
    // Render synth
    synth.renderNextBlock(*bufferToFill.buffer, midiBuffer, 
                          bufferToFill.startSample, bufferToFill.numSamples);
    
    // Apply effects
    juce::AudioBuffer<float> effectsBuffer;
    effectsBuffer.makeCopyOf(*bufferToFill.buffer);
    
    drive.process(effectsBuffer);
    delay.process(effectsBuffer);
    
    // Apply reverb (stereo processing)
    if (effectsBuffer.getNumChannels() >= 2) {
        reverb.processStereo(effectsBuffer.getWritePointer(0),
                             effectsBuffer.getWritePointer(1),
                             effectsBuffer.getNumSamples());
    } else if (effectsBuffer.getNumChannels() == 1) {
        reverb.processMono(effectsBuffer.getWritePointer(0),
                           effectsBuffer.getNumSamples());
    }
    
    // Copy processed audio back
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel) {
        bufferToFill.buffer->copyFrom(channel, bufferToFill.startSample,
                                       effectsBuffer, channel, 0, bufferToFill.numSamples);
    }
}

void SynthAudioSource::releaseResources() {
    // Nothing to release
}

void SynthAudioSource::setParameters(const SynthParameters& params) {
    currentParams = params;
    
    for (int i = 0; i < synth.getNumVoices(); ++i) {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            voice->setParameters(params);
        }
    }
}

void SynthAudioSource::setDriveParameters(DriveType type, float amount, float mix, float output) {
    drive.setType(type);
    drive.setDrive(amount);
    drive.setMix(mix);
    drive.setOutputGain(output);
}

void SynthAudioSource::setDelayParameters(float time, float feedback, float mix) {
    delay.setDelayTime(time);
    delay.setFeedback(feedback);
    delay.setMix(mix);
}

void SynthAudioSource::setReverbParameters(float roomSize, float damping, float wetLevel, float dryLevel, float width) {
    juce::Reverb::Parameters params;
    params.roomSize = roomSize;
    params.damping = damping;
    params.wetLevel = wetLevel;
    params.dryLevel = dryLevel;
    params.width = width;
    params.freezeMode = 0.0f;
    reverb.setParameters(params);
}

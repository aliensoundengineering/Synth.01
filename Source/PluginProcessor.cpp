#include "PluginProcessor.h"
#include "PluginEditor.h"

CodingSoundsSynthProcessor::CodingSoundsSynthProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)) {
    
    // Add voices to synthesizer
    for (int i = 0; i < numVoices; ++i) {
        synth.addVoice(new SynthVoice());
    }
    
    // Add sound
    synth.addSound(new SynthSound());
}

CodingSoundsSynthProcessor::~CodingSoundsSynthProcessor() {
}

const juce::String CodingSoundsSynthProcessor::getName() const {
    return JucePlugin_Name;
}

bool CodingSoundsSynthProcessor::acceptsMidi() const {
    return true;
}

bool CodingSoundsSynthProcessor::producesMidi() const {
    return false;
}

bool CodingSoundsSynthProcessor::isMidiEffect() const {
    return false;
}

double CodingSoundsSynthProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int CodingSoundsSynthProcessor::getNumPrograms() {
    return 1;
}

int CodingSoundsSynthProcessor::getCurrentProgram() {
    return 0;
}

void CodingSoundsSynthProcessor::setCurrentProgram(int index) {
    juce::ignoreUnused(index);
}

const juce::String CodingSoundsSynthProcessor::getProgramName(int index) {
    juce::ignoreUnused(index);
    return {};
}

void CodingSoundsSynthProcessor::changeProgramName(int index, const juce::String& newName) {
    juce::ignoreUnused(index, newName);
}

void CodingSoundsSynthProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    // Prepare all voices
    for (int i = 0; i < synth.getNumVoices(); ++i) {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            voice->prepareToPlay(sampleRate, samplesPerBlock, 2);
        }
    }
    
    // Prepare effects
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;
    
    drive.prepare(spec);
    delay.prepare(spec);
    reverb.setSampleRate(sampleRate);
}

void CodingSoundsSynthProcessor::releaseResources() {
}

bool CodingSoundsSynthProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void CodingSoundsSynthProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                               juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Apply pending parameter updates (thread-safe)
    if (paramsNeedUpdate.exchange(false)) {
        std::lock_guard<std::mutex> lock(paramMutex);
        currentParams = pendingParams;
        for (int i = 0; i < synth.getNumVoices(); ++i) {
            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->setParameters(currentParams);
            }
        }
    }
    
    // Apply pending drive parameters
    if (driveParamsNeedUpdate.exchange(false)) {
        std::lock_guard<std::mutex> lock(paramMutex);
        drive.setType(pendingDriveParams.type);
        drive.setDrive(pendingDriveParams.amount);
        drive.setMix(pendingDriveParams.mix);
        drive.setOutputGain(pendingDriveParams.output);
    }
    
    // Apply pending delay parameters
    if (delayParamsNeedUpdate.exchange(false)) {
        std::lock_guard<std::mutex> lock(paramMutex);
        delay.setDelayTime(pendingDelayParams.time);
        delay.setFeedback(pendingDelayParams.feedback);
        delay.setMix(pendingDelayParams.mix);
    }
    
    // Apply pending reverb parameters
    if (reverbParamsNeedUpdate.exchange(false)) {
        std::lock_guard<std::mutex> lock(paramMutex);
        juce::Reverb::Parameters params;
        params.roomSize = pendingReverbParams.roomSize;
        params.damping = pendingReverbParams.damping;
        params.wetLevel = pendingReverbParams.wetLevel;
        params.dryLevel = pendingReverbParams.dryLevel;
        params.width = pendingReverbParams.width;
        params.freezeMode = 0.0f;
        reverb.setParameters(params);
    }

    // Process keyboard state for GUI keyboard
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    
    // Render synth
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    // Apply effects (only when active)
    if (driveActive.load(std::memory_order_relaxed))
        drive.process(buffer);

    if (delayActive.load(std::memory_order_relaxed))
        delay.process(buffer);

    if (reverbActive.load(std::memory_order_relaxed)) {
        if (buffer.getNumChannels() >= 2) {
            reverb.processStereo(buffer.getWritePointer(0),
                                 buffer.getWritePointer(1),
                                 buffer.getNumSamples());
        } else if (buffer.getNumChannels() == 1) {
            reverb.processMono(buffer.getWritePointer(0),
                               buffer.getNumSamples());
        }
    }
}

bool CodingSoundsSynthProcessor::hasEditor() const {
    return true;
}

juce::AudioProcessorEditor* CodingSoundsSynthProcessor::createEditor() {
    return new CodingSoundsSynthEditor(*this);
}

void CodingSoundsSynthProcessor::getStateInformation(juce::MemoryBlock& destData) {
    // Salva lo stato del synth (per preset FL Studio)
    juce::MemoryOutputStream stream(destData, true);
    
    stream.writeFloat(currentParams.osc1Gain);
    stream.writeFloat(currentParams.osc2Gain);
    stream.writeFloat(currentParams.filterCutoff);
    stream.writeFloat(currentParams.filterResonance);
    stream.writeFloat(currentParams.masterGain);
    // Aggiungi altri parametri se necessario
}

void CodingSoundsSynthProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // Carica lo stato del synth (per preset FL Studio)
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    
    if (stream.getDataSize() > 0) {
        currentParams.osc1Gain = stream.readFloat();
        currentParams.osc2Gain = stream.readFloat();
        currentParams.filterCutoff = stream.readFloat();
        currentParams.filterResonance = stream.readFloat();
        currentParams.masterGain = stream.readFloat();
        
        setParameters(currentParams);
    }
}

void CodingSoundsSynthProcessor::setParameters(const SynthParameters& params) {
    std::lock_guard<std::mutex> lock(paramMutex);
    pendingParams = params;
    paramsNeedUpdate = true;
}

void CodingSoundsSynthProcessor::setDriveParameters(DriveType type, float amount, float mix, float output) {
    std::lock_guard<std::mutex> lock(paramMutex);
    pendingDriveParams.type = type;
    pendingDriveParams.amount = amount;
    pendingDriveParams.mix = mix;
    pendingDriveParams.output = output;
    driveParamsNeedUpdate = true;
}

void CodingSoundsSynthProcessor::setDelayParameters(float time, float feedback, float mix) {
    std::lock_guard<std::mutex> lock(paramMutex);
    pendingDelayParams.time = time;
    pendingDelayParams.feedback = feedback;
    pendingDelayParams.mix = mix;
    delayParamsNeedUpdate = true;
}

void CodingSoundsSynthProcessor::setReverbParameters(float roomSize, float damping, float wetLevel, float dryLevel, float width) {
    std::lock_guard<std::mutex> lock(paramMutex);
    pendingReverbParams.roomSize = roomSize;
    pendingReverbParams.damping = damping;
    pendingReverbParams.wetLevel = wetLevel;
    pendingReverbParams.dryLevel = dryLevel;
    pendingReverbParams.width = width;
    reverbParamsNeedUpdate = true;
}

void CodingSoundsSynthProcessor::setDriveActive(bool active) {
    driveActive.store(active, std::memory_order_relaxed);
}

void CodingSoundsSynthProcessor::setDelayActive(bool active) {
    delayActive.store(active, std::memory_order_relaxed);
}

void CodingSoundsSynthProcessor::setReverbActive(bool active) {
    reverbActive.store(active, std::memory_order_relaxed);
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new CodingSoundsSynthProcessor();
}

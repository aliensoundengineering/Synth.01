cmake_minimum_required(VERSION 3.22)

project(Synth.01 VERSION 0.0.1)

# Add JUCE folder
option(JUCE_BUILD_EXAMPLES "Build JUCE examples" OFF)
option(JUCE_BUILD_EXTRAS "Build JUCE extras" OFF)
add_subdirectory(JUCE)

# Plugin formats: VST3 + Standalone everywhere; AU only on Apple platforms.
set(SYNTH01_FORMATS VST3 Standalone)
if(APPLE)
    list(APPEND SYNTH01_FORMATS AU)
endif()

juce_add_plugin(
    ${PROJECT_NAME}
    PLUGIN_MANUFACTURER_CODE AlienSoundEngineering
    PLUGIN_CODE Csyn
    FORMATS ${SYNTH01_FORMATS}
    COMPANY_NAME "AlienSoundEngineering"
    COMPANY_WEBSITE "https://aliensoundengineering.com"
    COMPANY_EMAIL "ase@aliensoundengineering.com"
    BUNDLE_ID com.aliensoundengineering.synth01
    PRODUCT_NAME "AlienSoundEngineering Synth.01"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
)

target_sources(${PROJECT_NAME} PRIVATE
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
    Source/Synth/SynthSound.h
    Source/Synth/SynthVoice.h
    Source/Synth/SynthVoice.cpp
    Source/Synth/Oscillator.h
    Source/Synth/Oscillator.cpp
    Source/Synth/AdsrEnvelope.h
    Source/Synth/AdsrEnvelope.cpp
    Source/Synth/LfoGenerator.h
    Source/Synth/LfoGenerator.cpp
    Source/Synth/Filter.h
    Source/Synth/Filter.cpp
    Source/Effects/DriveDistortion.h
    Source/Effects/DriveDistortion.cpp
    Source/Effects/DelayLine.h
    Source/Effects/DelayLine.cpp
    Source/Preset/PresetManager.h
    Source/Preset/PresetManager.cpp
    Source/GUI/CustomLookAndFeel.h
    Source/GUI/CustomLookAndFeel.cpp
    Source/GUI/OscillatorComponent.h
    Source/GUI/OscillatorComponent.cpp
    Source/GUI/AdsrComponent.h
    Source/GUI/AdsrComponent.cpp
    Source/GUI/LfoComponent.h
    Source/GUI/LfoComponent.cpp
    Source/GUI/FilterComponent.h
    Source/GUI/FilterComponent.cpp
    Source/GUI/EffectsComponent.h
    Source/GUI/EffectsComponent.cpp
    Source/GUI/PresetComponent.h
    Source/GUI/PresetComponent.cpp
    Source/SynthAudioSource.h
    Source/SynthAudioSource.cpp
)

# Setup compiler
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE "/utf-8")
endif()

# Definizioni per evitare conflitti VST2/VST3
target_compile_definitions(${PROJECT_NAME} PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    juce::juce_core
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
)

juce_generate_juce_header(${PROJECT_NAME})
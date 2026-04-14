cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SeqFX VERSION 0.1.0)

# Fetch JUCE
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.12
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(SeqFX
    COMPANY_NAME "SeqFX"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Sefx
    PLUGIN_CODE Sfx1
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "SeqFX"
)

target_sources(SeqFX
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginEditor.cpp
        src/dsp/FilterEngine.cpp
        src/dsp/DelayEngine.cpp
        src/dsp/ReverbEngine.cpp
        src/dsp/DistortionEngine.cpp
        src/dsp/ChorusEngine.cpp
        src/dsp/SidechainCompressorEngine.cpp
        src/sequencer/SequencerState.cpp
        src/sequencer/SequencerEngine.cpp
        src/ui/SequencerMatrixComponent.cpp
        src/ui/EffectSection.cpp
        src/ui/LaneComponent.cpp
        src/ui/EffectControls.cpp
        src/ui/ToolbarComponent.cpp
)

target_include_directories(SeqFX
    PRIVATE
        src
        src/dsp
        src/sequencer
        src/ui
)

target_link_libraries(SeqFX
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_compile_definitions(SeqFX
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# ------------------------------------------------------------------------------
# Packaging / Installers
# ------------------------------------------------------------------------------
set(CPACK_PACKAGE_NAME "SeqFX")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "SeqFX")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SeqFX - Sequenced Multi-FX Plugin")

if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "SeqFX")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
    include(CPack)
endif()

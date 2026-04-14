#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "dsp/FilterEngine.h"
#include "dsp/DelayEngine.h"
#include "dsp/ReverbEngine.h"
#include "dsp/DistortionEngine.h"
#include "dsp/ChorusEngine.h"
#include "sequencer/SequencerEngine.h"
#include "sequencer/SequencerState.h"

class PluginEditor;

class PluginProcessor  : public juce::AudioProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    SequencerState& getSequencerState() { return sequencerState; }
    SequencerEngine& getSequencerEngine() { return sequencerEngine; }

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    juce::AudioProcessorValueTreeState apvts;

    FilterEngine filterEngine;
    DelayEngine delayEngine;
    ReverbEngine reverbEngine;
    DistortionEngine distortionEngine;
    ChorusEngine chorusEngine;

    SequencerState sequencerState;
    SequencerEngine sequencerEngine;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};

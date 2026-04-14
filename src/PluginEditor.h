#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class PluginProcessor;
class SequencerMatrixComponent;
class EffectControls;

class PluginEditor  : public juce::AudioProcessorEditor
{
public:
    PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PluginProcessor& processorRef;
    std::unique_ptr<SequencerMatrixComponent> matrix;
    std::unique_ptr<EffectControls> controls;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};

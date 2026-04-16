#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class PluginProcessor;
class SequencerMatrixComponent;
class EffectControls;
class ToolbarComponent;

class PluginEditor  : public juce::AudioProcessorEditor
{
public:
    PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    bool keyPressed (const juce::KeyPress&) override;

    SequencerMatrixComponent* getMatrix() const noexcept { return matrix.get(); }
    juce::UndoManager& getUndoManager() noexcept { return undoManager; }

private:
    PluginProcessor& processorRef;
    std::unique_ptr<ToolbarComponent> toolbar;
    std::unique_ptr<SequencerMatrixComponent> matrix;
    std::unique_ptr<EffectControls> controls;
    juce::UndoManager undoManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};

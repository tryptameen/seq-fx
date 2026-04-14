#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>
#include <vector>

class PluginProcessor;
class EffectSection;

class SequencerMatrixComponent  : public juce::Component,
                                   public juce::Timer
{
public:
    SequencerMatrixComponent (PluginProcessor& proc);
    ~SequencerMatrixComponent() override;

    void paint (juce::Graphics& g) override;
    void paintOverChildren (juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

private:
    PluginProcessor& processor;
    std::vector<std::unique_ptr<EffectSection>> sections;
    float lastPlayheadX { -1.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerMatrixComponent)
};

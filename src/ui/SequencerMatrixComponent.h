#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>
#include <vector>

class PluginProcessor;
class LaneComponent;

class SequencerMatrixComponent  : public juce::Component
{
public:
    SequencerMatrixComponent (PluginProcessor& proc);
    ~SequencerMatrixComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    PluginProcessor& processor;
    std::vector<std::unique_ptr<LaneComponent>> lanes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerMatrixComponent)
};

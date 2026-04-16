#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>
#include <vector>

class PluginProcessor;
class EffectSection;
class GateLaneComponent;

class SequencerMatrixComponent  : public juce::Component,
                                   public juce::Timer
{
public:
    SequencerMatrixComponent (PluginProcessor& proc, juce::UndoManager& um);
    ~SequencerMatrixComponent() override;

    void paint (juce::Graphics& g) override;
    void paintOverChildren (juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;
    void refreshAll();
    void mouseDown (const juce::MouseEvent& e) override;

private:
    PluginProcessor& processor;
    juce::UndoManager& undoManager;
    std::unique_ptr<GateLaneComponent> gateLane;
    std::vector<std::unique_ptr<EffectSection>> sections;
    float lastPlayheadX { -1.0f };
    float triggerFlash { 0.0f };

    void rebuildSections();
    void moveEffectUp (int posInOrder);
    void moveEffectDown (int posInOrder);
    void showContextMenu();
    void savePattern();
    void loadPattern();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerMatrixComponent)
};

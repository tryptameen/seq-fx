#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "sequencer/SequencerState.h"

class GateLaneComponent  : public juce::Component
{
public:
    GateLaneComponent (SequencerState& state, juce::UndoManager& um);

    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;

    void refresh() { repaint(); }
    void showContextMenu();

private:
    SequencerState& seqState;
    juce::UndoManager& undoManager;
    std::vector<float> dragSnapshot;

    int xToStep (int x) const;
    float yToValue (int y) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GateLaneComponent)
};

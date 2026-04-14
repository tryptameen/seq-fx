#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "sequencer/SequencerState.h"

class GateLaneComponent  : public juce::Component
{
public:
    GateLaneComponent (SequencerState& state);

    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;

    void refresh() { repaint(); }

private:
    SequencerState& seqState;

    int xToStep (int x) const;
    float yToValue (int y) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GateLaneComponent)
};

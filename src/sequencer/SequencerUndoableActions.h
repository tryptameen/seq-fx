#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "SequencerState.h"
#include <vector>

class SetLaneValuesAction  : public juce::UndoableAction
{
public:
    SetLaneValuesAction (SequencerState& state, int lane,
                         std::vector<float> oldValues,
                         std::vector<float> newValues)
        : seqState (state), lane (lane),
          oldValues (std::move (oldValues)),
          newValues (std::move (newValues))
    {
    }

    bool perform() override
    {
        for (size_t i = 0; i < newValues.size(); ++i)
            seqState.setStepValue (lane, static_cast<int> (i), newValues[i]);
        return true;
    }

    bool undo() override
    {
        for (size_t i = 0; i < oldValues.size(); ++i)
            seqState.setStepValue (lane, static_cast<int> (i), oldValues[i]);
        return true;
    }

private:
    SequencerState& seqState;
    int lane;
    std::vector<float> oldValues;
    std::vector<float> newValues;
};

class SetGateValuesAction  : public juce::UndoableAction
{
public:
    SetGateValuesAction (SequencerState& state,
                         std::vector<float> oldValues,
                         std::vector<float> newValues)
        : seqState (state),
          oldValues (std::move (oldValues)),
          newValues (std::move (newValues))
    {
    }

    bool perform() override
    {
        for (size_t i = 0; i < newValues.size(); ++i)
            seqState.setGateValue (static_cast<int> (i), newValues[i]);
        return true;
    }

    bool undo() override
    {
        for (size_t i = 0; i < oldValues.size(); ++i)
            seqState.setGateValue (static_cast<int> (i), oldValues[i]);
        return true;
    }

private:
    SequencerState& seqState;
    std::vector<float> oldValues;
    std::vector<float> newValues;
};

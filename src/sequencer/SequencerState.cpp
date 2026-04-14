#include "SequencerState.h"

SequencerState::SequencerState()
{
    setGrid (1, 16);
}

void SequencerState::setGrid (int bars, int stepsPerBar)
{
    numBars = juce::jlimit (1, 32, bars);
    stepsPerBar = (stepsPerBar == 4 || stepsPerBar == 8 || stepsPerBar == 16) ? stepsPerBar : 16;
    totalSteps = numBars * stepsPerBar;

    for (auto& lane : lanes)
    {
        lane.resize (static_cast<size_t> (totalSteps));
        for (auto& v : lane)
            v = 0.0f;
    }
}

float SequencerState::getStepValue (int lane, int step) const noexcept
{
    if (lane < 0 || lane >= ParameterMatrix::NumLanes) return 0.0f;
    if (step < 0 || step >= totalSteps) return 0.0f;
    return lanes[static_cast<size_t> (lane)][static_cast<size_t> (step)];
}

void SequencerState::setStepValue (int lane, int step, float normalizedValue)
{
    if (lane < 0 || lane >= ParameterMatrix::NumLanes) return;
    if (step < 0 || step >= totalSteps) return;
    lanes[static_cast<size_t> (lane)][static_cast<size_t> (step)] = juce::jlimit (0.0f, 1.0f, normalizedValue);
}

void SequencerState::clearLane (int lane)
{
    if (lane < 0 || lane >= ParameterMatrix::NumLanes) return;
    std::fill (lanes[static_cast<size_t> (lane)].begin(), lanes[static_cast<size_t> (lane)].end(), 0.0f);
}

void SequencerState::clearAll()
{
    for (int i = 0; i < ParameterMatrix::NumLanes; ++i)
        clearLane (i);
}

void SequencerState::writeToMemoryBlock (juce::MemoryBlock& mb) const
{
    juce::MemoryOutputStream stream (mb, false);
    stream.writeInt (numBars);
    stream.writeInt (stepsPerBar);
    stream.writeInt (totalSteps);
    for (const auto& lane : lanes)
    {
        stream.write (lane.data(), static_cast<size_t> (totalSteps) * sizeof (float));
    }
}

void SequencerState::readFromMemoryBlock (const juce::MemoryBlock& mb)
{
    juce::MemoryInputStream stream (mb, false);
    if (stream.getNumBytesRemaining() < 12)
        return;

    int newBars = stream.readInt();
    int newStepsPerBar = stream.readInt();
    int newTotalSteps = stream.readInt();

    setGrid (newBars, newStepsPerBar);

    const auto expectedBytes = static_cast<size_t> (ParameterMatrix::NumLanes * newTotalSteps * sizeof (float));
    if (static_cast<size_t> (stream.getNumBytesRemaining()) < expectedBytes)
        return;

    for (auto& lane : lanes)
    {
        stream.read (lane.data(), static_cast<size_t> (newTotalSteps) * sizeof (float));
    }
}

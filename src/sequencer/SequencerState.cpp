#include "SequencerState.h"

SequencerState::SequencerState()
{
    setGrid (1, 16);
    for (int i = 0; i < ParameterMatrix::NumEffects; ++i)
    {
        effectOrder[static_cast<size_t> (i)] = i;
        bypass[static_cast<size_t> (i)] = false;
        solo[static_cast<size_t> (i)] = false;
    }
}

void SequencerState::setGrid (int bars, int stepsPerBar)
{
    numBars = juce::jlimit (1, 32, bars);
    this->stepsPerBar = (stepsPerBar == 4 || stepsPerBar == 8 || stepsPerBar == 16) ? stepsPerBar : 16;
    totalSteps = numBars * this->stepsPerBar;

    for (auto& lane : lanes)
    {
        size_t oldSize = lane.size();
        lane.resize (static_cast<size_t> (totalSteps));
        if (lane.size() > oldSize)
            std::fill (lane.begin() + static_cast<std::ptrdiff_t> (oldSize), lane.end(), 0.0f);
    }

    size_t oldGateSize = gateValues.size();
    gateValues.resize (static_cast<size_t> (totalSteps));
    if (gateValues.size() > oldGateSize)
        std::fill (gateValues.begin() + static_cast<std::ptrdiff_t> (oldGateSize), gateValues.end(), 1.0f);
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

float SequencerState::getGateValue (int step) const noexcept
{
    if (step < 0 || step >= totalSteps) return 1.0f;
    return gateValues[static_cast<size_t> (step)];
}

void SequencerState::setGateValue (int step, float normalizedValue)
{
    if (step < 0 || step >= totalSteps) return;
    gateValues[static_cast<size_t> (step)] = juce::jlimit (0.0f, 1.0f, normalizedValue);
}

void SequencerState::clearGateValues()
{
    std::fill (gateValues.begin(), gateValues.end(), 1.0f);
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
    clearGateValues();
}

void SequencerState::setEffectOrder (const std::array<int, ParameterMatrix::NumEffects>& order)
{
    effectOrder = order;
}

void SequencerState::swapEffects (int posA, int posB)
{
    if (posA < 0 || posA >= ParameterMatrix::NumEffects) return;
    if (posB < 0 || posB >= ParameterMatrix::NumEffects) return;
    std::swap (effectOrder[static_cast<size_t> (posA)], effectOrder[static_cast<size_t> (posB)]);
}

bool SequencerState::isBypassed (int effect) const noexcept
{
    if (effect < 0 || effect >= ParameterMatrix::NumEffects) return false;
    return bypass[static_cast<size_t> (effect)];
}

void SequencerState::setBypassed (int effect, bool bypassed)
{
    if (effect < 0 || effect >= ParameterMatrix::NumEffects) return;
    bypass[static_cast<size_t> (effect)] = bypassed;
}

bool SequencerState::isSoloed (int effect) const noexcept
{
    if (effect < 0 || effect >= ParameterMatrix::NumEffects) return false;
    return solo[static_cast<size_t> (effect)];
}

void SequencerState::setSoloed (int effect, bool soloed)
{
    if (effect < 0 || effect >= ParameterMatrix::NumEffects) return;
    solo[static_cast<size_t> (effect)] = soloed;
}

bool SequencerState::anySoloActive() const noexcept
{
    for (bool s : solo)
        if (s) return true;
    return false;
}

void SequencerState::writeToMemoryBlock (juce::MemoryBlock& mb) const
{
    juce::MemoryOutputStream stream (mb, false);
    stream.writeInt (numBars);
    stream.writeInt (stepsPerBar);
    stream.writeInt (totalSteps);
    for (const auto& lane : lanes)
        stream.write (lane.data(), static_cast<size_t> (totalSteps) * sizeof (float));
    stream.write (gateValues.data(), static_cast<size_t> (totalSteps) * sizeof (float));

    stream.write (effectOrder.data(), effectOrder.size() * sizeof (int));
    for (bool b : bypass)
        stream.writeByte (b ? 1 : 0);
    for (bool s : solo)
        stream.writeByte (s ? 1 : 0);

    stream.writeInt (snapMode);
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
        stream.read (lane.data(), static_cast<size_t> (newTotalSteps) * sizeof (float));

    if (static_cast<size_t> (stream.getNumBytesRemaining()) >= static_cast<size_t> (newTotalSteps * sizeof (float)))
        stream.read (gateValues.data(), static_cast<size_t> (newTotalSteps) * sizeof (float));
    else
        clearGateValues();

    if (stream.getNumBytesRemaining() >= static_cast<juce::int64> (effectOrder.size() * sizeof (int)))
        stream.read (effectOrder.data(), effectOrder.size() * sizeof (int));

    for (bool& b : bypass)
        if (stream.getNumBytesRemaining() > 0)
            b = stream.readByte() != 0;

    for (bool& s : solo)
        if (stream.getNumBytesRemaining() > 0)
            s = stream.readByte() != 0;

    if (stream.getNumBytesRemaining() >= static_cast<juce::int64> (sizeof (int)))
        snapMode = stream.readInt();
}

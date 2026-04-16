#pragma once
#include "ParameterMatrix.h"
#include <array>
#include <vector>
#include <juce_core/juce_core.h>

class SequencerState
{
public:
    SequencerState();

    void setGrid (int bars, int stepsPerBar);
    int getNumBars() const noexcept { return numBars; }
    int getStepsPerBar() const noexcept { return stepsPerBar; }
    int getTotalSteps() const noexcept { return totalSteps; }

    float getStepValue (int lane, int step) const noexcept;
    void setStepValue (int lane, int step, float normalizedValue);

    float getGateValue (int step) const noexcept;
    void setGateValue (int step, float normalizedValue);
    void clearGateValues();

    void clearLane (int lane);
    void clearAll();

    // Effect routing
    const std::array<int, ParameterMatrix::NumEffects>& getEffectOrder() const noexcept { return effectOrder; }
    void setEffectOrder (const std::array<int, ParameterMatrix::NumEffects>& order);
    void swapEffects (int posA, int posB);

    bool isBypassed (int effect) const noexcept;
    void setBypassed (int effect, bool bypassed);

    bool isSoloed (int effect) const noexcept;
    void setSoloed (int effect, bool soloed);

    bool anySoloActive() const noexcept;

    int getSnapMode() const noexcept { return snapMode; }
    void setSnapMode (int mode) { snapMode = juce::jlimit (0, 2, mode); }

    void writeToMemoryBlock (juce::MemoryBlock& mb) const;
    void readFromMemoryBlock (const juce::MemoryBlock& mb);

private:
    int numBars { 1 };
    int stepsPerBar { 16 };
    int totalSteps { 16 };
    int snapMode { 0 };
    std::array<std::vector<float>, ParameterMatrix::NumLanes> lanes;
    std::vector<float> gateValues;

    std::array<int, ParameterMatrix::NumEffects> effectOrder;
    std::array<bool, ParameterMatrix::NumEffects> bypass;
    std::array<bool, ParameterMatrix::NumEffects> solo;
};

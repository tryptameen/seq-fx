#pragma once
#include "SequencerState.h"
#include <array>
#include <atomic>

class SequencerEngine
{
public:
    enum class Interpolation
    {
        Hold = 0,
        Glide
    };

    enum class SyncMode
    {
        Host = 0,
        MidiNote
    };

    SequencerEngine();

    void setState (const SequencerState* statePtr) { state = statePtr; }
    void setInterpolation (Interpolation interp) { interpolation = interp; }
    void setSyncMode (SyncMode mode) { syncMode = mode; }
    void setPlayheadPPQ (double ppq);
    void setMidiStepIndex (int step);
    void setSwing (float swing01);

    void prepare (double sampleRate);
    void processBlock (int numSamples);

    float getSmoothedValue (int lane) const noexcept { return smoothedValues[static_cast<size_t> (lane)]; }

    double getCurrentStepFraction() const;
    int getCurrentStep() const noexcept { return currentStep; }
    bool wasStepTriggered() const noexcept { return stepTriggered; }

private:
    const SequencerState* state { nullptr };
    Interpolation interpolation { Interpolation::Hold };
    SyncMode syncMode { SyncMode::Host };

    double sampleRate { 44100.0 };
    double lastPpq { -1.0 };
    int currentStep { -1 };
    int midiStepIndex { 0 };
    float swing { 0.0f };
    std::atomic<bool> stepTriggered { false };

    std::array<float, ParameterMatrix::NumLanes> targetValues {};
    std::array<float, ParameterMatrix::NumLanes> smoothedValues {};

    void updateTargets (int stepIndex);
    void updateSmoothing (int numSamples);
};

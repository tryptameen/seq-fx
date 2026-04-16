#include "SequencerEngine.h"
#include <cmath>

SequencerEngine::SequencerEngine()
{
    targetValues.fill (0.0f);
    smoothedValues.fill (0.0f);
}

void SequencerEngine::prepare (double newSampleRate)
{
    sampleRate = newSampleRate;
}

void SequencerEngine::setPlayheadPPQ (double ppq)
{
    lastPpq = ppq;
}

void SequencerEngine::setMidiStepIndex (int step)
{
    midiStepIndex = step;
}

void SequencerEngine::setSwing (float swing01)
{
    swing = juce::jlimit (0.0f, 1.0f, swing01);
}

double SequencerEngine::getCurrentStepFraction() const
{
    if (state == nullptr || state->getTotalSteps() <= 0)
        return -1.0;

    if (syncMode == SyncMode::MidiNote)
        return static_cast<double> (currentStep);

    if (lastPpq < 0.0)
        return -1.0;

    const double stepsPerQuarterNote = state->getStepsPerBar() / 4.0;
    const double stepDuration = 1.0 / stepsPerQuarterNote;

    double adjustedPpq = lastPpq;
    if (swing > 0.0f)
    {
        double rawStep = lastPpq * stepsPerQuarterNote;
        int stepIndex = static_cast<int> (std::floor (rawStep));
        double frac = rawStep - stepIndex;

        if (stepIndex % 2 != 0)
        {
            // Odd step delayed by swing
            double swingDelay = swing * 0.66 * stepDuration;
            if (frac * stepDuration < swingDelay)
                adjustedPpq -= swingDelay;
        }
    }

    double currentStepF = adjustedPpq * stepsPerQuarterNote;
    const double totalStepsD = static_cast<double> (state->getTotalSteps());
    return std::fmod (currentStepF, totalStepsD);
}

void SequencerEngine::processBlock (int numSamples)
{
    stepTriggered = false;

    if (state == nullptr)
    {
        updateSmoothing (numSamples);
        return;
    }

    const int totalSteps = state->getTotalSteps();
    if (totalSteps <= 0)
    {
        updateSmoothing (numSamples);
        return;
    }

    int stepIndex = currentStep;

    if (syncMode == SyncMode::Host)
    {
        if (lastPpq < 0.0)
        {
            updateSmoothing (numSamples);
            return;
        }

        const double stepsPerQuarterNote = state->getStepsPerBar() / 4.0;

        double adjustedPpq = lastPpq;
        if (swing > 0.0f)
        {
            double rawStep = lastPpq * stepsPerQuarterNote;
            int sIdx = static_cast<int> (std::floor (rawStep));
            double frac = rawStep - sIdx;
            double stepDuration = 1.0 / stepsPerQuarterNote;

            if (sIdx % 2 != 0)
            {
                double swingDelay = swing * 0.66 * stepDuration;
                if (frac * stepDuration < swingDelay)
                    adjustedPpq -= swingDelay;
            }
        }

        double currentStepF = adjustedPpq * stepsPerQuarterNote;
        stepIndex = static_cast<int> (std::floor (currentStepF)) % totalSteps;
        if (stepIndex < 0) stepIndex += totalSteps;
    }
    else // MidiNote
    {
        stepIndex = midiStepIndex % totalSteps;
        if (stepIndex < 0) stepIndex += totalSteps;
    }

    if (stepIndex != currentStep)
    {
        currentStep = stepIndex;
        float gate = state->getGateValue (stepIndex);
        bool allowTrigger = (gate >= 1.0f) || (juce::Random::getSystemRandom().nextFloat() < gate);

        if (allowTrigger)
        {
            updateTargets (stepIndex);
            stepTriggered = true;
        }
    }

    updateSmoothing (numSamples);
}

void SequencerEngine::updateTargets (int stepIndex)
{
    for (int lane = 0; lane < ParameterMatrix::NumLanes; ++lane)
    {
        float norm = state->getStepValue (lane, stepIndex);
        targetValues[lane] = ParameterMatrix::normalizedToReal (lane, norm);
    }
}

void SequencerEngine::updateSmoothing (int numSamples)
{
    float timeConstant = (interpolation == Interpolation::Glide) ? 0.100f : 0.005f;
    float coef = 1.0f - std::exp (-1.0f / (timeConstant * static_cast<float> (sampleRate)));

    for (int lane = 0; lane < ParameterMatrix::NumLanes; ++lane)
    {
        float current = smoothedValues[lane];
        float target = targetValues[lane];

        if (std::abs (target - current) < 0.0001f)
        {
            smoothedValues[lane] = target;
            continue;
        }

        for (int i = 0; i < numSamples; ++i)
            current += (target - current) * coef;

        smoothedValues[lane] = current;
    }
}

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

void SequencerEngine::processBlock (int numSamples)
{
    if (state == nullptr)
    {
        updateSmoothing (numSamples);
        return;
    }

    const int totalSteps = state->getTotalSteps();
    if (totalSteps <= 0 || lastPpq < 0.0)
    {
        updateSmoothing (numSamples);
        return;
    }

    const double stepsPerQuarterNote = state->getStepsPerBar() / 4.0;
    const double currentStepF = lastPpq * stepsPerQuarterNote;
    const int stepIndex = static_cast<int> (std::floor (currentStepF)) % totalSteps;

    if (stepIndex != currentStep)
    {
        currentStep = stepIndex;
        updateTargets (stepIndex);
    }

    updateSmoothing (numSamples);
}

void SequencerEngine::updateTargets (int stepIndex)
{
    const int totalSteps = state->getTotalSteps();

    for (int lane = 0; lane < ParameterMatrix::NumLanes; ++lane)
    {
        float norm = state->getStepValue (lane, stepIndex);

        if (interpolation == Interpolation::Glide)
        {
            int nextStep = (stepIndex + 1) % totalSteps;
            float nextNorm = state->getStepValue (lane, nextStep);
            // Fractional progress within current step handled in smoothing? 
            // For simple glide, we just target the next value and let smoothing interpolate over step duration.
            // Actually better: target the current value, and smoothing constant determines glide speed.
            // Let's use current value for hold/glide, but with faster smoothing for glide.
            (void) nextNorm;
        }

        targetValues[lane] = ParameterMatrix::normalizedToReal (lane, norm);
    }
}

void SequencerEngine::updateSmoothing (int numSamples)
{
    const float coef = (interpolation == Interpolation::Glide) ? 0.005f : 0.05f;

    for (int lane = 0; lane < ParameterMatrix::NumLanes; ++lane)
    {
        float current = smoothedValues[lane];
        float target = targetValues[lane];

        if (std::abs (target - current) < 0.001f)
        {
            smoothedValues[lane] = target;
            continue;
        }

        // one-pole smoothing per sample
        for (int i = 0; i < numSamples; ++i)
            current += (target - current) * coef;

        smoothedValues[lane] = current;
    }
}

#include "ReverbEngine.h"

ReverbEngine::ReverbEngine()
{
    params.roomSize = 0.5f;
    params.damping = 0.5f;
    params.wetLevel = 0.33f;
    params.dryLevel = 0.67f;
    params.width = 1.0f;
    params.freezeMode = 0.0f;
    reverb.setParameters (params);
}

void ReverbEngine::prepare (const juce::dsp::ProcessSpec& spec)
{
    reverb.prepare (spec);
}

void ReverbEngine::reset()
{
    reverb.reset();
}

void ReverbEngine::setSize (float size01)
{
    params.roomSize = juce::jlimit (0.0f, 1.0f, size01);
    reverb.setParameters (params);
}

void ReverbEngine::setDamping (float damping01)
{
    params.damping = juce::jlimit (0.0f, 1.0f, damping01);
    reverb.setParameters (params);
}

void ReverbEngine::setMix (float mix01)
{
    mix = juce::jlimit (0.0f, 1.0f, mix01);
    params.wetLevel = mix;
    params.dryLevel = 1.0f - mix;
    reverb.setParameters (params);
}

void ReverbEngine::process (juce::AudioBuffer<float>& buffer)
{
    if (mix <= 0.0f)
        return;

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    reverb.process (context);
}

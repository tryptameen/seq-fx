#include "FilterEngine.h"

FilterEngine::FilterEngine()
{
    filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
}

void FilterEngine::prepare (const juce::dsp::ProcessSpec& spec)
{
    currentSpec = spec;
    filter.prepare (spec);
    filter.setCutoffFrequency (currentCutoff);
    filter.setResonance (currentResonance);
    wetBuffer.setSize (static_cast<int> (spec.numChannels), static_cast<int> (spec.maximumBlockSize));
}

void FilterEngine::reset()
{
    filter.reset();
}

void FilterEngine::setCutoff (float cutoffHz)
{
    currentCutoff = juce::jlimit (20.0f, 20000.0f, cutoffHz);
    filter.setCutoffFrequency (currentCutoff);
}

void FilterEngine::setResonance (float resonance)
{
    currentResonance = juce::jmax (0.001f, resonance);
    filter.setResonance (currentResonance);
}

void FilterEngine::setMix (float mix01)
{
    mix = juce::jlimit (0.0f, 1.0f, mix01);
}

void FilterEngine::process (juce::AudioBuffer<float>& buffer)
{
    if (mix <= 0.0f)
        return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    if (mix >= 1.0f)
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        filter.process (context);
        return;
    }

    wetBuffer.setSize (numChannels, numSamples, false, false, true);
    for (int ch = 0; ch < numChannels; ++ch)
        wetBuffer.copyFrom (ch, 0, buffer, ch, 0, numSamples);

    juce::dsp::AudioBlock<float> wetBlock (wetBuffer);
    juce::dsp::ProcessContextReplacing<float> wetContext (wetBlock);
    filter.process (wetContext);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* dry = buffer.getWritePointer (ch);
        auto* wet = wetBuffer.getReadPointer (ch);
        for (int i = 0; i < numSamples; ++i)
            dry[i] = dry[i] * (1.0f - mix) + wet[i] * mix;
    }
}

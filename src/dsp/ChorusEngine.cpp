#include "ChorusEngine.h"

ChorusEngine::ChorusEngine()
{
    chorus.setCentreDelay (7.0f);
}

void ChorusEngine::prepare (const juce::dsp::ProcessSpec& spec)
{
    chorus.prepare (spec);
}

void ChorusEngine::reset()
{
    chorus.reset();
}

void ChorusEngine::setRate (float rateHz)
{
    chorus.setRate (juce::jlimit (0.01f, 20.0f, rateHz));
}

void ChorusEngine::setDepth (float depth01)
{
    chorus.setDepth (juce::jlimit (0.0f, 1.0f, depth01));
}

void ChorusEngine::setMix (float mix01)
{
    mix = juce::jlimit (0.0f, 1.0f, mix01);
}

void ChorusEngine::process (juce::AudioBuffer<float>& buffer)
{
    if (mix <= 0.0f)
        return;

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);

    if (mix >= 1.0f)
    {
        chorus.process (context);
        return;
    }

    juce::AudioBuffer<float> wet (buffer.getNumChannels(), buffer.getNumSamples());
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        wet.copyFrom (ch, 0, buffer, ch, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> wetBlock (wet);
    juce::dsp::ProcessContextReplacing<float> wetContext (wetBlock);
    chorus.process (wetContext);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* dry = buffer.getWritePointer (ch);
        auto* wetPtr = wet.getReadPointer (ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            dry[i] = dry[i] * (1.0f - mix) + wetPtr[i] * mix;
    }
}

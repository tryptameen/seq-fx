#include "DistortionEngine.h"

DistortionEngine::DistortionEngine()
{
    toneFilter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
}

void DistortionEngine::prepare (const juce::dsp::ProcessSpec& spec)
{
    currentSpec = spec;
    toneFilter.prepare (spec);
    wetBuffer.setSize (static_cast<int> (spec.numChannels), static_cast<int> (spec.maximumBlockSize));
}

void DistortionEngine::reset()
{
    toneFilter.reset();
}

void DistortionEngine::setDrive (float drive01)
{
    drive = 1.0f + drive01 * 19.0f; // 1x to 20x pre-gain
}

void DistortionEngine::setTone (float toneHz)
{
    toneFilter.setCutoffFrequency (juce::jlimit (20.0f, 20000.0f, toneHz));
}

void DistortionEngine::setMix (float mix01)
{
    mix = juce::jlimit (0.0f, 1.0f, mix01);
}

void DistortionEngine::process (juce::AudioBuffer<float>& buffer)
{
    if (mix <= 0.0f)
        return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    wetBuffer.setSize (numChannels, numSamples, false, false, true);
    for (int ch = 0; ch < numChannels; ++ch)
        wetBuffer.copyFrom (ch, 0, buffer, ch, 0, numSamples);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* wet = wetBuffer.getWritePointer (ch);
        for (int i = 0; i < numSamples; ++i)
            wet[i] = std::tanh (wet[i] * drive);
    }

    juce::dsp::AudioBlock<float> wetBlock (wetBuffer);
    juce::dsp::ProcessContextReplacing<float> wetContext (wetBlock);
    toneFilter.process (wetContext);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* dry = buffer.getWritePointer (ch);
        auto* wet = wetBuffer.getReadPointer (ch);
        for (int i = 0; i < numSamples; ++i)
            dry[i] = dry[i] * (1.0f - mix) + wet[i] * mix;
    }
}

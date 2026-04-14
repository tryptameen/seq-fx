#include "DelayEngine.h"

DelayEngine::DelayEngine() = default;

void DelayEngine::prepare (double sampleRate, int maxBlockSize)
{
    sr = sampleRate;
    const int maxDelaySamples = static_cast<int> (std::ceil (sr * 3.0)) + maxBlockSize;
    delayBuffer.setSize (2, maxDelaySamples);
    delayBuffer.clear();
    writePos = 0;
    readPos = 0.0f;
    updateDelayTime();
}

void DelayEngine::reset()
{
    delayBuffer.clear();
    writePos = 0;
    readPos = 0.0f;
}

void DelayEngine::setTimeMs (float timeMs)
{
    delayTimeMs = juce::jlimit (1.0f, 3000.0f, timeMs);
    updateDelayTime();
}

void DelayEngine::setFeedback (float feedback01)
{
    feedback = juce::jlimit (0.0f, 0.99f, feedback01);
}

void DelayEngine::setMix (float mix01)
{
    mix = juce::jlimit (0.0f, 1.0f, mix01);
}

void DelayEngine::updateDelayTime()
{
    readPos = writePos - static_cast<float> (delayTimeMs * 0.001 * sr);
    while (readPos < 0)
        readPos += static_cast<float> (delayBuffer.getNumSamples());
}

void DelayEngine::process (juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    const int delaySize = delayBuffer.getNumSamples();

    if (delaySize == 0)
        return;

    for (int i = 0; i < numSamples; ++i)
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            const int readIndex = static_cast<int> (std::floor (readPos)) % delaySize;
            const float frac = readPos - std::floor (readPos);
            const int nextIndex = (readIndex + 1) % delaySize;

            const float delayed = delayBuffer.getSample (ch, readIndex) * (1.0f - frac)
                                + delayBuffer.getSample (ch, nextIndex) * frac;

            const float input = buffer.getSample (ch, i);
            const float wet = delayed;
            const float dry = input;

            buffer.setSample (ch, i, dry * (1.0f - mix) + wet * mix);

            const float writeSample = input + delayed * feedback;
            delayBuffer.setSample (ch, writePos, writeSample);
        }

        readPos += 1.0f;
        if (readPos >= delaySize)
            readPos -= static_cast<float> (delaySize);

        writePos = (writePos + 1) % delaySize;
    }
}

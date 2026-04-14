#include "SidechainCompressorEngine.h"
#include <cmath>

SidechainCompressorEngine::SidechainCompressorEngine() = default;

void SidechainCompressorEngine::prepare (double sampleRate, int maxBlockSize)
{
    (void) maxBlockSize;
    envelope.assign (2, 0.0f);
    attackCoeff  = std::exp (-1.0f / (0.005f  * static_cast<float> (sampleRate)));
    releaseCoeff = std::exp (-1.0f / (0.100f * static_cast<float> (sampleRate)));
}

void SidechainCompressorEngine::reset()
{
    std::fill (envelope.begin(), envelope.end(), 0.0f);
}

void SidechainCompressorEngine::setThreshold (float db)
{
    thresholdDb = juce::jlimit (-60.0f, 0.0f, db);
}

void SidechainCompressorEngine::setRatio (float r)
{
    ratio = juce::jlimit (1.0f, 20.0f, r);
}

void SidechainCompressorEngine::setMix (float mix01)
{
    mix = juce::jlimit (0.0f, 1.0f, mix01);
}

void SidechainCompressorEngine::process (juce::AudioBuffer<float>& mainBuffer,
                                         const juce::AudioBuffer<float>& sidechainBuffer)
{
    if (mix <= 0.0f)
        return;

    const int numChannels = mainBuffer.getNumChannels();
    const int numSamples  = mainBuffer.getNumSamples();

    if (envelope.size() < static_cast<size_t> (numChannels))
        envelope.resize (static_cast<size_t> (numChannels), 0.0f);

    juce::AudioBuffer<float> compressed (numChannels, numSamples);
    for (int ch = 0; ch < numChannels; ++ch)
        compressed.copyFrom (ch, 0, mainBuffer, ch, 0, numSamples);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* main = compressed.getWritePointer (ch);
        auto* side = sidechainBuffer.getReadPointer (juce::jmin (ch, sidechainBuffer.getNumChannels() - 1));

        for (int i = 0; i < numSamples; ++i)
        {
            float sideLevel = std::abs (side[i]);
            float coeff = sideLevel > envelope[static_cast<size_t> (ch)] ? attackCoeff : releaseCoeff;
            envelope[static_cast<size_t> (ch)] = coeff * envelope[static_cast<size_t> (ch)] + (1.0f - coeff) * sideLevel;

            float envDb = 20.0f * std::log10 (envelope[static_cast<size_t> (ch)] + 1e-10f);
            float gainDb = 0.0f;
            if (envDb > thresholdDb)
                gainDb = (thresholdDb - envDb) * (1.0f - 1.0f / ratio);

            float gain = std::pow (10.0f, gainDb / 20.0f);
            main[i] *= gain;
        }
    }

    if (mix >= 1.0f)
    {
        for (int ch = 0; ch < numChannels; ++ch)
            mainBuffer.copyFrom (ch, 0, compressed, ch, 0, numSamples);
        return;
    }

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* dry = mainBuffer.getWritePointer (ch);
        auto* wet = compressed.getReadPointer (ch);
        for (int i = 0; i < numSamples; ++i)
            dry[i] = dry[i] * (1.0f - mix) + wet[i] * mix;
    }
}

#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>

class SidechainCompressorEngine
{
public:
    SidechainCompressorEngine();

    void prepare (double sampleRate, int maxBlockSize);
    void reset();

    void setThreshold (float thresholdDb);
    void setRatio (float ratio);
    void setMix (float mix01);

    void process (juce::AudioBuffer<float>& mainBuffer,
                  const juce::AudioBuffer<float>& sidechainBuffer);

private:
    float thresholdDb { -20.0f };
    float ratio { 4.0f };
    float mix { 0.0f };
    float attackCoeff { 0.0f };
    float releaseCoeff { 0.0f };
    std::vector<float> envelope;
};

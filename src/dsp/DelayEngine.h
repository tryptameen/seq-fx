#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

class DelayEngine
{
public:
    DelayEngine();

    void prepare (double sampleRate, int maxBlockSize);
    void reset();

    void setTimeMs (float timeMs);
    void setFeedback (float feedback01);
    void setMix (float mix01);

    void process (juce::AudioBuffer<float>& buffer);

private:
    void updateDelayTime();

    juce::AudioBuffer<float> delayBuffer;
    double sr { 44100.0 };
    int writePos { 0 };
    float delayTimeMs { 250.0f };
    float feedback { 0.3f };
    float mix { 0.35f };
    float readPos { 0.0f };
};

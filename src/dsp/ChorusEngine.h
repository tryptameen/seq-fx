#pragma once
#include <juce_dsp/juce_dsp.h>

class ChorusEngine
{
public:
    ChorusEngine();

    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();

    void setRate (float rateHz);
    void setDepth (float depth01);
    void setMix (float mix01);

    void process (juce::AudioBuffer<float>& buffer);

private:
    juce::dsp::Chorus<float> chorus;
    float mix { 0.0f };
};

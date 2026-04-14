#pragma once
#include <juce_dsp/juce_dsp.h>

class ReverbEngine
{
public:
    ReverbEngine();

    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();

    void setSize (float size01);
    void setDamping (float damping01);
    void setMix (float mix01);

    void process (juce::AudioBuffer<float>& buffer);

private:
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters params;
    float mix { 0.3f };
};

#pragma once
#include <juce_dsp/juce_dsp.h>

class DistortionEngine
{
public:
    DistortionEngine();

    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();

    void setDrive (float drive01);
    void setTone (float toneHz);
    void setMix (float mix01);

    void process (juce::AudioBuffer<float>& buffer);

private:
    juce::dsp::StateVariableTPTFilter<float> toneFilter;
    juce::AudioBuffer<float> wetBuffer;
    float mix { 0.0f };
    float drive { 1.0f };
    juce::dsp::ProcessSpec currentSpec {};
};

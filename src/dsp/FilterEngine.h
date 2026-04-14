#pragma once
#include <juce_dsp/juce_dsp.h>

class FilterEngine
{
public:
    FilterEngine();

    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();

    void setCutoff (float cutoffHz);
    void setResonance (float resonance);
    void setMix (float mix01);

    void process (juce::AudioBuffer<float>& buffer);

private:
    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::AudioBuffer<float> wetBuffer;
    float mix { 0.0f };
    float currentCutoff { 1000.0f };
    float currentResonance { 1.0f / juce::MathConstants<float>::sqrt2 };
    juce::dsp::ProcessSpec currentSpec {};
};

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "sequencer/ParameterMatrix.h"

class PluginProcessor;

class EffectControls  : public juce::Component
{
public:
    EffectControls (PluginProcessor& proc);
    ~EffectControls() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    PluginProcessor& processor;

    struct EffectGroup
    {
        juce::Label title;
        std::vector<std::unique_ptr<juce::Slider>> sliders;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
        std::vector<std::unique_ptr<juce::Label>> labels;
    };

    std::array<EffectGroup, ParameterMatrix::NumEffects> groups;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectControls)
};

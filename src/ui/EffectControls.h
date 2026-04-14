#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

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
    std::vector<std::unique_ptr<juce::Slider>> sliders;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
    std::vector<std::unique_ptr<juce::Label>> labels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectControls)
};

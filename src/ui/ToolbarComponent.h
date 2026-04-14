#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "sequencer/SequencerState.h"

class PluginProcessor;

class ToolbarComponent  : public juce::Component
{
public:
    ToolbarComponent (PluginProcessor& proc);
    ~ToolbarComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    PluginProcessor& processor;
    SequencerState& seqState;

    juce::Slider barsSlider;
    juce::ComboBox stepsBox;
    juce::TextButton interpolationButton { "Hold" };
    juce::Slider swingSlider;
    juce::Slider inputGainSlider;
    juce::Slider outputGainSlider;
    juce::TextButton randomizeButton { "Randomize" };
    juce::TextButton clearButton { "Clear" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> barsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> stepsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> swingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;

    void onInterpolationClicked();
    void onRandomizeClicked();
    void onClearClicked();
    void updateInterpolationButton();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToolbarComponent)
};

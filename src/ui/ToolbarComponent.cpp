#include "ToolbarComponent.h"
#include "PluginProcessor.h"

ToolbarComponent::ToolbarComponent (PluginProcessor& proc)
    : processor (proc), seqState (proc.getSequencerState())
{
    barsSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    barsSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, 40, 20);
    barsSlider.setRange (1, 32, 1);
    addAndMakeVisible (barsSlider);
    barsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor.getAPVTS(), "bars", barsSlider);

    stepsBox.addItem ("4 steps/bar", 1);
    stepsBox.addItem ("8 steps/bar", 2);
    stepsBox.addItem ("16 steps/bar", 3);
    addAndMakeVisible (stepsBox);
    stepsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        processor.getAPVTS(), "stepsPerBar", stepsBox);

    interpolationButton.onClick = [this] { onInterpolationClicked(); };
    addAndMakeVisible (interpolationButton);
    updateInterpolationButton();

    swingSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    swingSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, 40, 20);
    swingSlider.setRange (0.0, 1.0, 0.01);
    addAndMakeVisible (swingSlider);
    swingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor.getAPVTS(), "swing", swingSlider);

    inputGainSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    inputGainSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, 50, 20);
    inputGainSlider.setRange (-48.0, 12.0, 0.1);
    addAndMakeVisible (inputGainSlider);
    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor.getAPVTS(), "inputGain", inputGainSlider);

    outputGainSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    outputGainSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, 50, 20);
    outputGainSlider.setRange (-48.0, 12.0, 0.1);
    addAndMakeVisible (outputGainSlider);
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor.getAPVTS(), "outputGain", outputGainSlider);

    randomizeButton.onClick = [this] { onRandomizeClicked(); };
    addAndMakeVisible (randomizeButton);

    clearButton.onClick = [this] { onClearClicked(); };
    addAndMakeVisible (clearButton);
}

ToolbarComponent::~ToolbarComponent() = default;

void ToolbarComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xFF1A1A1A));
    g.setColour (juce::Colour (0xFF333333));
    g.drawRect (getLocalBounds().toFloat(), 1.0f);
}

void ToolbarComponent::resized()
{
    auto area = getLocalBounds().reduced (8);
    barsSlider.setBounds (area.removeFromLeft (140));
    stepsBox.setBounds (area.removeFromLeft (100).reduced (4));
    interpolationButton.setBounds (area.removeFromLeft (65).reduced (4));
    swingSlider.setBounds (area.removeFromLeft (120));
    inputGainSlider.setBounds (area.removeFromLeft (120).reduced (4));
    outputGainSlider.setBounds (area.removeFromLeft (120).reduced (4));
    randomizeButton.setBounds (area.removeFromRight (85).reduced (4));
    clearButton.setBounds (area.removeFromRight (65).reduced (4));
}

void ToolbarComponent::onInterpolationClicked()
{
    auto* param = processor.getAPVTS().getRawParameterValue ("interpolation");
    if (param == nullptr) return;

    float newVal = (*param > 0.5f) ? 0.0f : 1.0f;
    processor.getAPVTS().getParameter ("interpolation")->setValueNotifyingHost (newVal);
    updateInterpolationButton();
}

void ToolbarComponent::updateInterpolationButton()
{
    auto* param = processor.getAPVTS().getRawParameterValue ("interpolation");
    bool isGlide = (param != nullptr && *param > 0.5f);
    interpolationButton.setButtonText (isGlide ? "Glide" : "Hold");
}

void ToolbarComponent::onRandomizeClicked()
{
    juce::Random rng;
    for (int lane = 0; lane < ParameterMatrix::NumLanes; ++lane)
        for (int s = 0; s < seqState.getTotalSteps(); ++s)
            seqState.setStepValue (lane, s, rng.nextFloat());
    for (int s = 0; s < seqState.getTotalSteps(); ++s)
        seqState.setGateValue (s, rng.nextFloat());
    repaint();
}

void ToolbarComponent::onClearClicked()
{
    seqState.clearAll();
    repaint();
}

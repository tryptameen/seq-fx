#include "EffectControls.h"
#include "PluginProcessor.h"

EffectControls::EffectControls (PluginProcessor& proc)
    : processor (proc)
{
    for (int i = 0; i < ParameterMatrix::NumLanes; ++i)
    {
        auto slider = std::make_unique<juce::Slider> (juce::Slider::RotaryHorizontalVerticalDrag,
                                                       juce::Slider::TextBoxBelow);
        slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 14);
        slider->setRange (ParameterMatrix::Lanes[static_cast<size_t> (i)].minValue, ParameterMatrix::Lanes[static_cast<size_t> (i)].maxValue);
        slider->setValue (ParameterMatrix::Lanes[static_cast<size_t> (i)].defaultValue);
        addAndMakeVisible (slider.get());

        auto attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            processor.getAPVTS(), ParameterMatrix::Lanes[static_cast<size_t> (i)].id, *slider);

        auto label = std::make_unique<juce::Label> ();
        label->setText (ParameterMatrix::Lanes[static_cast<size_t> (i)].name, juce::dontSendNotification);
        label->setJustificationType (juce::Justification::centred);
        addAndMakeVisible (label.get());

        sliders.push_back (std::move (slider));
        attachments.push_back (std::move (attachment));
        labels.push_back (std::move (label));
    }
}

EffectControls::~EffectControls() = default;

void EffectControls::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xFF1E1E1E));
    g.setColour (juce::Colour (0xFF333333));
    g.drawRect (getLocalBounds().toFloat(), 1.0f);
}

void EffectControls::resized()
{
    auto area = getLocalBounds().reduced (8);
    int knobW = juce::jlimit (50, 80, area.getWidth() / ParameterMatrix::NumLanes);
    int knobH = 90;

    int startX = (area.getWidth() - (ParameterMatrix::NumLanes * knobW)) / 2;
    int y = area.getCentreY() - knobH / 2;

    for (size_t i = 0; i < sliders.size(); ++i)
    {
        sliders[i]->setBounds (startX + static_cast<int> (i) * knobW, y + 14, knobW, knobH - 14);
        labels[i]->setBounds (startX + static_cast<int> (i) * knobW, y - 2, knobW, 18);
    }
}

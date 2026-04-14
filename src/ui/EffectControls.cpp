#include "EffectControls.h"
#include "PluginProcessor.h"

EffectControls::EffectControls (PluginProcessor& proc)
    : processor (proc)
{
    for (int effect = 0; effect < ParameterMatrix::NumEffects; ++effect)
    {
        groups[effect].title.setText (ParameterMatrix::EffectNames[effect], juce::dontSendNotification);
        groups[effect].title.setJustificationType (juce::Justification::centred);
        groups[effect].title.setFont (juce::Font (14.0f, juce::Font::bold));
        addAndMakeVisible (groups[effect].title);

        for (int lane = 0; lane < ParameterMatrix::LanesPerEffect; ++lane)
        {
            int laneIndex = effect * ParameterMatrix::LanesPerEffect + lane;
            const auto& info = ParameterMatrix::Lanes[static_cast<size_t> (laneIndex)];

            auto slider = std::make_unique<juce::Slider> (juce::Slider::RotaryHorizontalVerticalDrag,
                                                           juce::Slider::TextBoxBelow);
            slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 14);
            slider->setRange (info.minValue, info.maxValue);
            slider->setValue (info.defaultValue);
            addAndMakeVisible (slider.get());

            auto attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                processor.getAPVTS(), info.id, *slider);

            auto label = std::make_unique<juce::Label> ();
            label->setText (info.name, juce::dontSendNotification);
            label->setJustificationType (juce::Justification::centred);
            addAndMakeVisible (label.get());

            groups[effect].sliders.push_back (std::move (slider));
            groups[effect].attachments.push_back (std::move (attachment));
            groups[effect].labels.push_back (std::move (label));
        }
    }
}

EffectControls::~EffectControls() = default;

void EffectControls::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xFF1E1E1E));

    auto bounds = getLocalBounds().toFloat();
    g.setColour (juce::Colour (0xFF333333));
    g.drawRect (bounds, 1.0f);

    // Draw dividers between groups
    g.setColour (juce::Colour (0xFF2A2A2A));
    float sectionW = bounds.getWidth() / ParameterMatrix::NumEffects;
    for (int i = 1; i < ParameterMatrix::NumEffects; ++i)
    {
        float x = i * sectionW;
        g.drawVerticalLine (static_cast<int> (x), bounds.getY() + 4.0f, bounds.getBottom() - 4.0f);
    }
}

void EffectControls::resized()
{
    auto area = getLocalBounds().reduced (8);
    int sectionW = area.getWidth() / ParameterMatrix::NumEffects;

    for (int effect = 0; effect < ParameterMatrix::NumEffects; ++effect)
    {
        auto section = area.removeFromLeft (sectionW).reduced (4, 0);
        groups[effect].title.setBounds (section.removeFromTop (20));

        int knobW = juce::jlimit (40, 70, section.getWidth() / ParameterMatrix::LanesPerEffect);
        int knobH = section.getHeight();
        int startX = (section.getWidth() - (ParameterMatrix::LanesPerEffect * knobW)) / 2;

        for (size_t i = 0; i < groups[effect].sliders.size(); ++i)
        {
            int x = section.getX() + startX + static_cast<int> (i) * knobW;
            groups[effect].labels[i]->setBounds (x, section.getY(), knobW, 16);
            groups[effect].sliders[i]->setBounds (x, section.getY() + 16, knobW, knobH - 16);
        }
    }
}

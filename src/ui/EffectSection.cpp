#include "EffectSection.h"
#include "LaneComponent.h"
#include "PluginProcessor.h"

EffectSection::EffectSection (int fxIndex, PluginProcessor& proc)
    : effectIndex (fxIndex)
{
    toggleButton.onClick = [this] { onToggleClicked(); };
    addAndMakeVisible (toggleButton);

    auto& seqState = proc.getSequencerState();
    for (int lane = 0; lane < ParameterMatrix::LanesPerEffect; ++lane)
    {
        int laneIndex = effectIndex * ParameterMatrix::LanesPerEffect + lane;
        auto l = std::make_unique<LaneComponent> (laneIndex, seqState);
        addAndMakeVisible (l.get());
        lanes.push_back (std::move (l));
    }
}

EffectSection::~EffectSection() = default;

void EffectSection::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.fillAll (juce::Colour (0xFF181818));

    // Header bar
    juce::Colour headerColour = juce::Colour::fromHSV (0.55f - effectIndex * 0.1f, 0.7f, 0.35f, 1.0f);
    g.setColour (headerColour);
    g.fillRect (bounds.removeFromTop (24.0f));

    // Header text
    g.setColour (juce::Colours::white);
    g.setFont (13.0f);
    g.drawText (ParameterMatrix::EffectNames[effectIndex], bounds.withTop (0).withHeight (24).reduced (28.0f, 0), juce::Justification::centredLeft);

    // Border
    g.setColour (juce::Colour (0xFF2A2A2A));
    g.drawRect (getLocalBounds().toFloat(), 1.0f);
}

void EffectSection::resized()
{
    auto area = getLocalBounds();
    toggleButton.setBounds (area.removeFromTop (24).removeFromLeft (24).reduced (2));

    if (! expanded || lanes.empty())
        return;

    int laneH = area.getHeight() / static_cast<int> (lanes.size());
    for (auto& lane : lanes)
        lane->setBounds (area.removeFromTop (laneH).reduced (0, 1));
}

void EffectSection::setExpanded (bool shouldBeExpanded)
{
    if (expanded == shouldBeExpanded)
        return;

    expanded = shouldBeExpanded;
    toggleButton.setButtonText (expanded ? "-" : "+");

    for (auto& lane : lanes)
        lane->setVisible (expanded);

    resized();
    repaint();
    if (auto* parent = getParentComponent())
        parent->resized();
}

void EffectSection::refreshLanes()
{
    for (auto& lane : lanes)
        lane->refresh();
}

void EffectSection::onToggleClicked()
{
    setExpanded (! expanded);
}

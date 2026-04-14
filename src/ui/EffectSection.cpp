#include "EffectSection.h"
#include "LaneComponent.h"
#include "PluginProcessor.h"

EffectSection::EffectSection (int fxIndex, PluginProcessor& proc)
    : effectIndex (fxIndex), processor (proc)
{
    toggleButton.onClick = [this] { onToggleClicked(); };
    addAndMakeVisible (toggleButton);

    upButton.onClick = [this] { if (onMoveUp) onMoveUp(); };
    addAndMakeVisible (upButton);

    downButton.onClick = [this] { if (onMoveDown) onMoveDown(); };
    addAndMakeVisible (downButton);

    bypassButton.onClick = [this] { onBypassClicked(); };
    addAndMakeVisible (bypassButton);

    soloButton.onClick = [this] { onSoloClicked(); };
    addAndMakeVisible (soloButton);

    auto& seqState = proc.getSequencerState();
    for (int lane = 0; lane < ParameterMatrix::LanesPerEffect; ++lane)
    {
        int laneIndex = effectIndex * ParameterMatrix::LanesPerEffect + lane;
        auto l = std::make_unique<LaneComponent> (laneIndex, seqState);
        addAndMakeVisible (l.get());
        lanes.push_back (std::move (l));
    }

    updateButtonStates();
}

EffectSection::~EffectSection() = default;

void EffectSection::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    g.fillAll (juce::Colour (0xFF181818));

    juce::Colour headerColour = juce::Colour::fromHSV (0.55f - effectIndex * 0.08f, 0.7f, 0.35f, 1.0f);
    auto header = bounds.removeFromTop (24.0f);
    g.setColour (headerColour);
    g.fillRect (header);

    auto& seqState = processor.getSequencerState();
    if (seqState.isBypassed (effectIndex))
    {
        g.setColour (juce::Colours::black.withAlpha (0.4f));
        g.fillRect (header);
    }
    else if (seqState.isSoloed (effectIndex))
    {
        g.setColour (juce::Colours::yellow.withAlpha (0.25f));
        g.fillRect (header);
    }

    g.setColour (juce::Colours::white);
    g.setFont (13.0f);
    g.drawText (ParameterMatrix::EffectNames[effectIndex],
                header.reduced (80.0f, 0).withLeft (header.getX() + 76),
                juce::Justification::centredLeft);

    g.setColour (juce::Colour (0xFF2A2A2A));
    g.drawRect (getLocalBounds().toFloat(), 1.0f);
}

void EffectSection::resized()
{
    auto area = getLocalBounds();
    auto header = area.removeFromTop (24);

    toggleButton.setBounds (header.removeFromLeft (24).reduced (2));
    upButton.setBounds (header.removeFromLeft (24).reduced (2));
    downButton.setBounds (header.removeFromLeft (24).reduced (2));
    soloButton.setBounds (header.removeFromRight (28).reduced (2));
    bypassButton.setBounds (header.removeFromRight (28).reduced (2));

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

void EffectSection::onBypassClicked()
{
    auto& seqState = processor.getSequencerState();
    seqState.setBypassed (effectIndex, ! seqState.isBypassed (effectIndex));
    updateButtonStates();
    repaint();
}

void EffectSection::onSoloClicked()
{
    auto& seqState = processor.getSequencerState();
    seqState.setSoloed (effectIndex, ! seqState.isSoloed (effectIndex));
    updateButtonStates();
    repaint();
}

void EffectSection::updateButtonStates()
{
    auto& seqState = processor.getSequencerState();
    bypassButton.setColour (juce::TextButton::buttonColourId,
                            seqState.isBypassed (effectIndex) ? juce::Colour (0xFF884444) : juce::Colour (0xFF444444));
    soloButton.setColour (juce::TextButton::buttonColourId,
                          seqState.isSoloed (effectIndex) ? juce::Colour (0xFF888844) : juce::Colour (0xFF444444));
}

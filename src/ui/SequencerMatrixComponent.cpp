#include "SequencerMatrixComponent.h"
#include "PluginProcessor.h"
#include "LaneComponent.h"

SequencerMatrixComponent::SequencerMatrixComponent (PluginProcessor& proc)
    : processor (proc)
{
    for (int i = 0; i < ParameterMatrix::NumLanes; ++i)
    {
        auto lane = std::make_unique<LaneComponent> (i, processor.getSequencerState());
        addAndMakeVisible (lane.get());
        lanes.push_back (std::move (lane));
    }
}

SequencerMatrixComponent::~SequencerMatrixComponent() = default;

void SequencerMatrixComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xFF151515));
}

void SequencerMatrixComponent::resized()
{
    auto area = getLocalBounds();
    const int laneHeight = area.getHeight() / ParameterMatrix::NumLanes;
    for (size_t i = 0; i < lanes.size(); ++i)
        lanes[i]->setBounds (area.removeFromTop (laneHeight).reduced (0, 1));
}

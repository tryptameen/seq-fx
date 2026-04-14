#include "SequencerMatrixComponent.h"
#include "PluginProcessor.h"
#include "EffectSection.h"
#include "GateLaneComponent.h"

SequencerMatrixComponent::SequencerMatrixComponent (PluginProcessor& proc)
    : processor (proc)
{
    gateLane = std::make_unique<GateLaneComponent> (proc.getSequencerState());
    addAndMakeVisible (gateLane.get());

    rebuildSections();
    startTimerHz (30);
}

SequencerMatrixComponent::~SequencerMatrixComponent()
{
    stopTimer();
}

void SequencerMatrixComponent::rebuildSections()
{
    sections.clear();
    const auto& order = processor.getSequencerState().getEffectOrder();

    for (int pos = 0; pos < ParameterMatrix::NumEffects; ++pos)
    {
        int effect = order[static_cast<size_t> (pos)];
        auto section = std::make_unique<EffectSection> (effect, processor);

        if (pos > 0)
            section->onMoveUp = [this, pos] { moveEffectUp (pos); };
        if (pos < ParameterMatrix::NumEffects - 1)
            section->onMoveDown = [this, pos] { moveEffectDown (pos); };

        addAndMakeVisible (section.get());
        sections.push_back (std::move (section));
    }
    resized();
    repaint();
}

void SequencerMatrixComponent::moveEffectUp (int posInOrder)
{
    if (posInOrder <= 0) return;
    auto& seqState = processor.getSequencerState();
    seqState.swapEffects (posInOrder, posInOrder - 1);
    rebuildSections();
}

void SequencerMatrixComponent::moveEffectDown (int posInOrder)
{
    if (posInOrder >= ParameterMatrix::NumEffects - 1) return;
    auto& seqState = processor.getSequencerState();
    seqState.swapEffects (posInOrder, posInOrder + 1);
    rebuildSections();
}

void SequencerMatrixComponent::timerCallback()
{
    float newX = -1.0f;
    auto& engine = processor.getSequencerEngine();
    double stepFrac = engine.getCurrentStepFraction();
    int totalSteps = processor.getSequencerState().getTotalSteps();

    if (stepFrac >= 0.0 && totalSteps > 0)
        newX = static_cast<float> (stepFrac / static_cast<double> (totalSteps) * getWidth());

    if (! juce::approximatelyEqual (newX, lastPlayheadX))
    {
        lastPlayheadX = newX;
        repaint();
    }

    gateLane->refresh();
    for (auto& section : sections)
        section->refreshLanes();
}

void SequencerMatrixComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xFF151515));
}

void SequencerMatrixComponent::paintOverChildren (juce::Graphics& g)
{
    if (lastPlayheadX >= 0.0f)
    {
        g.setColour (juce::Colours::white.withAlpha (0.8f));
        g.drawVerticalLine (static_cast<int> (lastPlayheadX), 0.0f, static_cast<float> (getHeight()));
    }
}

void SequencerMatrixComponent::resized()
{
    auto area = getLocalBounds();
    gateLane->setBounds (area.removeFromTop (28).reduced (0, 1));

    int expandedCount = 0;
    for (auto& section : sections)
        if (section->isExpanded())
            ++expandedCount;

    int headerHeight = 24;
    int fixedHeight = ParameterMatrix::NumEffects * headerHeight;
    int variableHeight = area.getHeight() - fixedHeight;

    if (expandedCount > 0 && variableHeight > 0)
    {
        for (auto& section : sections)
        {
            int h = headerHeight;
            if (section->isExpanded())
                h += (variableHeight * ParameterMatrix::LanesPerEffect) / (expandedCount * ParameterMatrix::LanesPerEffect);

            section->setBounds (area.removeFromTop (h).reduced (0, 1));
        }
    }
    else
    {
        int h = area.getHeight() / ParameterMatrix::NumEffects;
        for (auto& section : sections)
            section->setBounds (area.removeFromTop (h).reduced (0, 1));
    }
}

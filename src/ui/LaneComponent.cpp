#include "LaneComponent.h"
#include "sequencer/ParameterMatrix.h"
#include "sequencer/SequencerUndoableActions.h"
#include <cmath>

namespace
{
    std::vector<float> laneClipboard;
}

LaneComponent::LaneComponent (int laneIndex, SequencerState& state, juce::UndoManager& um)
    : lane (laneIndex), seqState (state), undoManager (um)
{
}

void LaneComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.fillAll (juce::Colour (0xFF222222));
    g.setColour (juce::Colour (0xFF333333));
    g.drawRect (bounds, 1.0f);

    const int steps = seqState.getTotalSteps();
    if (steps <= 0) return;

    const float stepW = bounds.getWidth() / static_cast<float> (steps);

    // Draw bar dividers
    g.setColour (juce::Colour (0xFF444444));
    const int spb = seqState.getStepsPerBar();
    for (int b = 1; b < steps / spb; ++b)
    {
        float x = b * spb * stepW;
        g.drawVerticalLine (static_cast<int> (x), bounds.getY(), bounds.getBottom());
    }

    // Draw step blocks
    for (int s = 0; s < steps; ++s)
    {
        float x = s * stepW;
        float val = seqState.getStepValue (lane, s);
        float h = val * bounds.getHeight();
        float y = bounds.getBottom() - h;

        g.setColour (juce::Colour::fromHSV (0.55f - lane * 0.06f, 0.8f, 0.9f, 1.0f));
        g.fillRect (x + 1.0f, y, stepW - 2.0f, h);
    }

    // Label
    g.setColour (juce::Colours::white);
    g.setFont (12.0f);
    g.drawText (ParameterMatrix::Lanes[lane].name, bounds.reduced (4.0f), juce::Justification::topLeft);
}

void LaneComponent::mouseDown (const juce::MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        showContextMenu();
        return;
    }
    dragSnapshot.clear();
    for (int s = 0; s < seqState.getTotalSteps(); ++s)
        dragSnapshot.push_back (seqState.getStepValue (lane, s));
    mouseDrag (e);
}

void LaneComponent::mouseUp (const juce::MouseEvent&)
{
    if (dragSnapshot.empty())
        return;

    std::vector<float> newValues;
    for (int s = 0; s < seqState.getTotalSteps(); ++s)
        newValues.push_back (seqState.getStepValue (lane, s));

    bool changed = false;
    for (size_t i = 0; i < dragSnapshot.size(); ++i)
        if (! juce::approximatelyEqual (dragSnapshot[i], newValues[i]))
        {
            changed = true;
            break;
        }

    if (changed)
    {
        undoManager.beginNewTransaction ("Edit lane");
        undoManager.perform (new SetLaneValuesAction (seqState, lane, dragSnapshot, newValues));
    }

    dragSnapshot.clear();
}

void LaneComponent::showContextMenu()
{
    juce::PopupMenu menu;
    menu.addItem ("Copy Lane", [this]
    {
        laneClipboard.clear();
        for (int s = 0; s < seqState.getTotalSteps(); ++s)
            laneClipboard.push_back (seqState.getStepValue (lane, s));
    });

    menu.addItem ("Paste Lane", [this]
    {
        if (laneClipboard.empty())
            return;
        int steps = juce::jmin (seqState.getTotalSteps(), static_cast<int> (laneClipboard.size()));
        for (int s = 0; s < steps; ++s)
            seqState.setStepValue (lane, s, laneClipboard[static_cast<size_t> (s)]);
        repaint();
    });

    menu.showMenuAsync (juce::PopupMenu::Options());
}

void LaneComponent::mouseDrag (const juce::MouseEvent& e)
{
    int step = xToStep (e.x);
    float val = yToValue (e.y);
    if (step >= 0 && step < seqState.getTotalSteps())
    {
        seqState.setStepValue (lane, step, val);
        repaint();
    }
}

int LaneComponent::xToStep (int x) const
{
    const int steps = seqState.getTotalSteps();
    if (steps <= 0) return -1;
    float norm = static_cast<float> (x) / getWidth();
    return juce::jlimit (0, steps - 1, static_cast<int> (std::floor (norm * steps)));
}

float LaneComponent::yToValue (int y) const
{
    float norm = 1.0f - (static_cast<float> (y) / getHeight());
    float val = juce::jlimit (0.0f, 1.0f, norm);
    int mode = seqState.getSnapMode();
    if (mode > 0)
    {
        float step = (mode == 1) ? 0.5f : 0.25f;
        val = juce::jlimit (0.0f, 1.0f, std::round (val / step) * step);
    }
    return val;
}

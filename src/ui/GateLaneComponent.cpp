#include "GateLaneComponent.h"

GateLaneComponent::GateLaneComponent (SequencerState& state)
    : seqState (state)
{
}

void GateLaneComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.fillAll (juce::Colour (0xFF222222));
    g.setColour (juce::Colour (0xFF333333));
    g.drawRect (bounds, 1.0f);

    const int steps = seqState.getTotalSteps();
    if (steps <= 0) return;

    const float stepW = bounds.getWidth() / static_cast<float> (steps);

    g.setColour (juce::Colour (0xFF444444));
    const int spb = seqState.getStepsPerBar();
    for (int b = 1; b < steps / spb; ++b)
    {
        float x = b * spb * stepW;
        g.drawVerticalLine (static_cast<int> (x), bounds.getY(), bounds.getBottom());
    }

    for (int s = 0; s < steps; ++s)
    {
        float x = s * stepW;
        float val = seqState.getGateValue (s);
        float h = val * bounds.getHeight();
        float y = bounds.getBottom() - h;

        g.setColour (juce::Colours::lightgreen.withAlpha (0.8f));
        g.fillRect (x + 1.0f, y, stepW - 2.0f, h);
    }

    g.setColour (juce::Colours::white);
    g.setFont (12.0f);
    g.drawText ("Gate / Probability", bounds.reduced (4.0f), juce::Justification::topLeft);
}

void GateLaneComponent::mouseDown (const juce::MouseEvent& e)
{
    mouseDrag (e);
}

void GateLaneComponent::mouseDrag (const juce::MouseEvent& e)
{
    int step = xToStep (e.x);
    float val = yToValue (e.y);
    if (step >= 0 && step < seqState.getTotalSteps())
    {
        seqState.setGateValue (step, val);
        repaint();
    }
}

int GateLaneComponent::xToStep (int x) const
{
    const int steps = seqState.getTotalSteps();
    if (steps <= 0) return -1;
    float norm = static_cast<float> (x) / getWidth();
    return juce::jlimit (0, steps - 1, static_cast<int> (std::floor (norm * steps)));
}

float GateLaneComponent::yToValue (int y) const
{
    float norm = 1.0f - (static_cast<float> (y) / getHeight());
    return juce::jlimit (0.0f, 1.0f, norm);
}

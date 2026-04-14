#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "ui/ToolbarComponent.h"
#include "ui/SequencerMatrixComponent.h"
#include "ui/EffectControls.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    toolbar = std::make_unique<ToolbarComponent> (p);
    addAndMakeVisible (toolbar.get());

    matrix = std::make_unique<SequencerMatrixComponent> (p);
    addAndMakeVisible (matrix.get());

    controls = std::make_unique<EffectControls> (p);
    addAndMakeVisible (controls.get());

    setSize (1000, 750);
}

PluginEditor::~PluginEditor() = default;

void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xFF1A1A1A));
}

void PluginEditor::resized()
{
    auto area = getLocalBounds().reduced (8);
    toolbar->setBounds (area.removeFromTop (44));
    controls->setBounds (area.removeFromBottom (160));
    matrix->setBounds (area.reduced (0, 4));
}

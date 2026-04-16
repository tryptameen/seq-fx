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

    matrix = std::make_unique<SequencerMatrixComponent> (p, undoManager);
    addAndMakeVisible (matrix.get());

    controls = std::make_unique<EffectControls> (p);
    addAndMakeVisible (controls.get());

    setResizable (true, true);
    setResizeLimits (600, 450, 2400, 1800);
    setSize (1000, 750);
    setWantsKeyboardFocus (true);
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

bool PluginEditor::keyPressed (const juce::KeyPress& k)
{
    if (k.isKeyCode ('z') && k.getModifiers().isCtrlDown())
    {
        if (k.getModifiers().isShiftDown())
            undoManager.redo();
        else
            undoManager.undo();
        if (matrix != nullptr)
            matrix->refreshAll();
        return true;
    }
    if (k.isKeyCode ('y') && k.getModifiers().isCtrlDown())
    {
        undoManager.redo();
        if (matrix != nullptr)
            matrix->refreshAll();
        return true;
    }
    return false;
}

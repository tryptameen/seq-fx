#include "SequencerMatrixComponent.h"
#include "PluginProcessor.h"
#include "EffectSection.h"
#include "GateLaneComponent.h"

SequencerMatrixComponent::SequencerMatrixComponent (PluginProcessor& proc, juce::UndoManager& um)
    : processor (proc), undoManager (um)
{
    gateLane = std::make_unique<GateLaneComponent> (proc.getSequencerState(), undoManager);
    addAndMakeVisible (gateLane.get());

    rebuildSections();
    startTimerHz (30);
}

SequencerMatrixComponent::~SequencerMatrixComponent()
{
    stopTimer();
}

void SequencerMatrixComponent::mouseDown (const juce::MouseEvent& e)
{
    if (e.mods.isPopupMenu())
        showContextMenu();
}

void SequencerMatrixComponent::showContextMenu()
{
    juce::PopupMenu menu;
    menu.addItem ("Save Pattern...", [this] { savePattern(); });
    menu.addItem ("Load Pattern...", [this] { loadPattern(); });
    menu.showMenuAsync (juce::PopupMenu::Options());
}

void SequencerMatrixComponent::savePattern()
{
    auto chooser = std::make_unique<juce::FileChooser> ("Save pattern", juce::File(), "*.seqfx");
    chooser->launchAsync (juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
        [this] (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file != juce::File())
            {
                juce::MemoryBlock data;
                processor.getStateInformation (data);
                file.replaceWithData (data.getData(), static_cast<int> (data.getSize()));
            }
        });
}

void SequencerMatrixComponent::loadPattern()
{
    auto chooser = std::make_unique<juce::FileChooser> ("Load pattern", juce::File(), "*.seqfx");
    chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this] (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file != juce::File())
            {
                juce::MemoryBlock data;
                if (file.loadFileAsData (data))
                {
                    processor.setStateInformation (data.getData(), static_cast<int> (data.getSize()));
                    refreshAll();
                }
            }
        });
}

void SequencerMatrixComponent::refreshAll()
{
    if (gateLane != nullptr)
        gateLane->refresh();
    for (auto& section : sections)
        section->refreshLanes();
    repaint();
}

void SequencerMatrixComponent::rebuildSections()
{
    sections.clear();
    const auto& order = processor.getSequencerState().getEffectOrder();

    for (int pos = 0; pos < ParameterMatrix::NumEffects; ++pos)
    {
        int effect = order[static_cast<size_t> (pos)];
        auto section = std::make_unique<EffectSection> (effect, processor, undoManager);

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

    bool needsRepaint = false;

    if (! juce::approximatelyEqual (newX, lastPlayheadX))
    {
        lastPlayheadX = newX;
        needsRepaint = true;
    }

    if (engine.wasStepTriggered())
    {
        triggerFlash = 1.0f;
        needsRepaint = true;
    }
    else if (triggerFlash > 0.0f)
    {
        triggerFlash -= 0.15f;
        if (triggerFlash < 0.0f)
            triggerFlash = 0.0f;
        needsRepaint = true;
    }

    if (needsRepaint)
        repaint();

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
    if (triggerFlash > 0.0f && lastPlayheadX >= 0.0f)
    {
        g.setColour (juce::Colours::white.withAlpha (triggerFlash * 0.25f));
        float w = juce::jmax (2.0f, static_cast<float> (getWidth()) / static_cast<float> (processor.getSequencerState().getTotalSteps()));
        g.fillRect (lastPlayheadX - w * 0.5f, 0.0f, w, static_cast<float> (getHeight()));
    }

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

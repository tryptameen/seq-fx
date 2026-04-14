#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>
#include <vector>

class LaneComponent;
class PluginProcessor;

class EffectSection  : public juce::Component
{
public:
    EffectSection (int effectIndex, PluginProcessor& proc);
    ~EffectSection() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    bool isExpanded() const noexcept { return expanded; }
    void setExpanded (bool shouldBeExpanded);

    void refreshLanes();

private:
    int effectIndex;
    bool expanded { true };
    juce::TextButton toggleButton { "-" };
    std::vector<std::unique_ptr<LaneComponent>> lanes;

    void onToggleClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectSection)
};

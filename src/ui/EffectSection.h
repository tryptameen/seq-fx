#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>
#include <vector>
#include <functional>

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

    std::function<void()> onMoveUp;
    std::function<void()> onMoveDown;

private:
    int effectIndex;
    PluginProcessor& processor;
    bool expanded { true };
    juce::TextButton toggleButton { "-" };
    juce::TextButton upButton { "\u2191" };   // ↑
    juce::TextButton downButton { "\u2193" }; // ↓
    juce::TextButton bypassButton { "B" };
    juce::TextButton soloButton { "S" };
    std::vector<std::unique_ptr<LaneComponent>> lanes;

    void onToggleClicked();
    void onBypassClicked();
    void onSoloClicked();
    void updateButtonStates();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectSection)
};

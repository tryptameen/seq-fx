#pragma once

class PluginProcessor;

namespace PresetManager
{
    int getNumPresets();
    const char* getPresetName (int index);
    void applyPreset (int index, PluginProcessor& proc);
}

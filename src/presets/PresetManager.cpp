#include "PresetManager.h"
#include "PluginProcessor.h"
#include "sequencer/ParameterMatrix.h"
#include <cmath>

namespace PresetManager
{
    static void setFloatParam (PluginProcessor& proc, const char* id, float realValue)
    {
        auto* p = proc.getAPVTS().getParameter (id);
        if (p != nullptr)
            p->setValueNotifyingHost (p->getNormalisableRange().convertTo0to1 (realValue));
    }

    static void setIntParam (PluginProcessor& proc, const char* id, int index)
    {
        auto* p = proc.getAPVTS().getParameter (id);
        if (p != nullptr)
            p->setValueNotifyingHost (p->getNormalisableRange().convertTo0to1 (static_cast<float> (index)));
    }

    // --------------------------------------------------------------------------
    static void applyInit (PluginProcessor& proc)
    {
        setIntParam  (proc, "bars", 1);
        setIntParam  (proc, "stepsPerBar", 2); // 16 steps
        setIntParam  (proc, "interpolation", 0);
        setFloatParam(proc, "swing", 0.0f);
        setFloatParam(proc, "inputGain", 0.0f);
        setFloatParam(proc, "outputGain", 0.0f);
        setFloatParam(proc, "dryWet", 1.0f);

        proc.getSequencerState().clearAll();
        for (int i = 0; i < ParameterMatrix::NumEffects; ++i)
        {
            proc.getSequencerState().setBypassed (i, false);
            proc.getSequencerState().setSoloed (i, false);
        }
    }

    static void applyFilterSweep (PluginProcessor& proc)
    {
        applyInit (proc);

        auto& seq = proc.getSequencerState();
        int steps = seq.getTotalSteps();
        for (int s = 0; s < steps; ++s)
        {
            float phase = static_cast<float> (s) / static_cast<float> (steps) * juce::MathConstants<float>::twoPi;
            float norm = (std::sin (phase) + 1.0f) * 0.5f; // 0..1 sine
            seq.setStepValue (ParameterMatrix::FilterCutoff, s, norm);
            seq.setStepValue (ParameterMatrix::FilterResonance, s,
                              ParameterMatrix::realToNormalized (ParameterMatrix::FilterResonance, 1.0f));
            seq.setStepValue (ParameterMatrix::FilterMix, s, 1.0f);
        }
    }

    static void applyStutterDelay (PluginProcessor& proc)
    {
        applyInit (proc);

        auto& seq = proc.getSequencerState();
        int steps = seq.getTotalSteps();
        for (int s = 0; s < steps; ++s)
        {
            seq.setStepValue (ParameterMatrix::DelayTime, s,
                              ParameterMatrix::realToNormalized (ParameterMatrix::DelayTime, 250.0f));
            seq.setStepValue (ParameterMatrix::DelayFeedback, s,
                              ParameterMatrix::realToNormalized (ParameterMatrix::DelayFeedback, 0.5f));
            seq.setStepValue (ParameterMatrix::DelayMix, s,
                              (s % 2 == 0) ? 1.0f : 0.0f);
        }
    }

    static void applySidechainPump (PluginProcessor& proc)
    {
        applyInit (proc);
        setIntParam (proc, "stepsPerBar", 1); // 8 steps

        auto& seq = proc.getSequencerState();
        int steps = seq.getTotalSteps();
        for (int s = 0; s < steps; ++s)
        {
            bool beat = (s % 4 == 0);
            seq.setStepValue (ParameterMatrix::CompressorThreshold, s,
                              ParameterMatrix::realToNormalized (ParameterMatrix::CompressorThreshold, beat ? -30.0f : 0.0f));
            seq.setStepValue (ParameterMatrix::CompressorRatio, s,
                              ParameterMatrix::realToNormalized (ParameterMatrix::CompressorRatio, 10.0f));
            seq.setStepValue (ParameterMatrix::CompressorMix, s, 1.0f);
        }
    }

    // --------------------------------------------------------------------------
    struct Preset
    {
        const char* name;
        void (*apply) (PluginProcessor&);
    };

    static const Preset presets[] = {
        { "Init",           applyInit },
        { "Filter Sweep",   applyFilterSweep },
        { "Stutter Delay",  applyStutterDelay },
        { "Sidechain Pump", applySidechainPump }
    };

    int getNumPresets()
    {
        return static_cast<int> (sizeof (presets) / sizeof (presets[0]));
    }

    const char* getPresetName (int index)
    {
        if (index < 0 || index >= getNumPresets())
            return "";
        return presets[index].name;
    }

    void applyPreset (int index, PluginProcessor& proc)
    {
        if (index < 0 || index >= getNumPresets())
            return;
        presets[index].apply (proc);
    }
}

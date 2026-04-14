#pragma once
#include <array>
#include <cmath>
#include <string>

namespace ParameterMatrix
{
    inline constexpr int LanesPerEffect = 3;
    inline constexpr int NumEffects = 5;
    inline constexpr int NumLanes = NumEffects * LanesPerEffect;

    enum LaneIndex
    {
        FilterCutoff = 0,
        FilterResonance,
        FilterMix,
        DelayTime,
        DelayFeedback,
        DelayMix,
        ReverbSize,
        ReverbDamping,
        ReverbMix,
        DistortionDrive,
        DistortionTone,
        DistortionMix,
        ChorusRate,
        ChorusDepth,
        ChorusMix
    };

    enum EffectIndex
    {
        FilterEffect = 0,
        DelayEffect,
        ReverbEffect,
        DistortionEffect,
        ChorusEffect
    };

    struct LaneInfo
    {
        const char* name;
        const char* id;
        float minValue;
        float maxValue;
        float defaultValue;
        bool isLogarithmic;
        int effectIndex;
    };

    inline const std::array<LaneInfo, NumLanes> Lanes = {{
        { "Filter Cutoff",    "filterCutoff",     20.0f,    20000.0f, 1000.0f, true,  FilterEffect },
        { "Filter Resonance", "filterResonance",  0.001f,   10.0f,    0.707f,  false, FilterEffect },
        { "Filter Mix",       "filterMix",        0.0f,     1.0f,     0.0f,    false, FilterEffect },
        { "Delay Time",       "delayTime",        1.0f,     3000.0f,  250.0f,  false, DelayEffect },
        { "Delay Feedback",   "delayFeedback",    0.0f,     0.95f,    0.3f,    false, DelayEffect },
        { "Delay Mix",        "delayMix",         0.0f,     1.0f,     0.35f,   false, DelayEffect },
        { "Reverb Size",      "reverbSize",       0.0f,     1.0f,     0.5f,    false, ReverbEffect },
        { "Reverb Damping",   "reverbDamping",    0.0f,     1.0f,     0.5f,    false, ReverbEffect },
        { "Reverb Mix",       "reverbMix",        0.0f,     1.0f,     0.3f,    false, ReverbEffect },
        { "Distortion Drive", "distortionDrive",  0.0f,     1.0f,     0.0f,    false, DistortionEffect },
        { "Distortion Tone",  "distortionTone",   200.0f,   8000.0f,  2000.0f, true,  DistortionEffect },
        { "Distortion Mix",   "distortionMix",    0.0f,     1.0f,     0.0f,    false, DistortionEffect },
        { "Chorus Rate",      "chorusRate",       0.01f,    20.0f,    0.5f,    false, ChorusEffect },
        { "Chorus Depth",     "chorusDepth",      0.0f,     1.0f,     0.3f,    false, ChorusEffect },
        { "Chorus Mix",       "chorusMix",        0.0f,     1.0f,     0.0f,    false, ChorusEffect }
    }};

    inline const char* EffectNames[NumEffects] = {
        "Filter", "Delay", "Reverb", "Distortion", "Chorus"
    };

    inline float normalizedToReal (int lane, float norm)
    {
        const auto& info = Lanes[static_cast<size_t> (lane)];
        if (info.isLogarithmic)
            return info.minValue * std::pow (info.maxValue / info.minValue, norm);
        return info.minValue + norm * (info.maxValue - info.minValue);
    }

    inline float realToNormalized (int lane, float real)
    {
        const auto& info = Lanes[static_cast<size_t> (lane)];
        if (info.isLogarithmic)
            return std::log (real / info.minValue) / std::log (info.maxValue / info.minValue);
        return (real - info.minValue) / (info.maxValue - info.minValue);
    }
}

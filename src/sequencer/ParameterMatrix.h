#pragma once
#include <array>
#include <cmath>
#include <string>

namespace ParameterMatrix
{
    inline constexpr int NumLanes = 9;

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
        ReverbMix
    };

    struct LaneInfo
    {
        const char* name;
        const char* id;
        float minValue;
        float maxValue;
        float defaultValue;
        bool isLogarithmic;
    };

    inline const std::array<LaneInfo, NumLanes> Lanes = {{
        { "Filter Cutoff",   "filterCutoff",    20.0f,   20000.0f, 1000.0f, true  },
        { "Filter Resonance","filterResonance", 0.0f,    10.0f,    0.707f,  false },
        { "Filter Mix",      "filterMix",       0.0f,    1.0f,     0.0f,    false },
        { "Delay Time",      "delayTime",       1.0f,    3000.0f,  250.0f,  false },
        { "Delay Feedback",  "delayFeedback",   0.0f,    0.95f,    0.3f,    false },
        { "Delay Mix",       "delayMix",        0.0f,    1.0f,     0.35f,   false },
        { "Reverb Size",     "reverbSize",      0.0f,    1.0f,     0.5f,    false },
        { "Reverb Damping",  "reverbDamping",   0.0f,    1.0f,     0.5f,    false },
        { "Reverb Mix",      "reverbMix",       0.0f,    1.0f,     0.3f,    false }
    }};

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

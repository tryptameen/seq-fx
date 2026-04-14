# SeqFX

A **sequenced multi-FX plugin** built with [JUCE](https://juce.com/). Run **Filter → Delay → Reverb** and automate every parameter via a host-synced step-sequencer (1–32 bars).

## Features

- **3 Effects in series**: State-Variable Filter, Delay, Reverb
- **9 sequencable parameter lanes**: Cutoff, Resonance, Mix, Time, Feedback, Size, Damping, etc.
- **Host-synced sequencer**: 1–32 bars, 4/8/16 steps per bar
- **Hold & Glide interpolation modes**
- **Full preset save/load** (step data travels with your DAW project)

## Formats

| OS | VST3 | AU | Standalone | Installer |
|---|---|---|---|---|
| Linux | ✅ | — | ✅ | `.deb` / `.tar.gz` |
| Windows | ✅ | — | ✅ | `.exe` (Inno Setup) |
| macOS | ✅ | ✅ | ✅ | `.dmg` |

> **macOS note**: SeqFX is not notarized. On first launch, right-click the app/plugin and select **Open**.

## Building

```bash
git clone --recursive https://github.com/tryptameen/seq-fx.git
cd seq-fx
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j$(nproc)
```

## License

MIT (or choose your own — see `LICENSE`)

#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginProcessor::PluginProcessor()
    : AudioProcessor (BusesProperties()
                      #if ! JucePlugin_IsMidiEffect
                       #if ! JucePlugin_IsSynth
                        .withInput  ("Input",      juce::AudioChannelSet::stereo(), true)
                        .withInput  ("Sidechain",  juce::AudioChannelSet::stereo(), true)
                       #endif
                        .withOutput ("Output",     juce::AudioChannelSet::stereo(), true)
                      #endif
                     ),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
    sequencerEngine.setState (&sequencerState);
}

PluginProcessor::~PluginProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    for (int i = 0; i < ParameterMatrix::NumLanes; ++i)
    {
        const auto& info = ParameterMatrix::Lanes[static_cast<size_t> (i)];
        if (info.isLogarithmic)
        {
            params.push_back (std::make_unique<juce::AudioParameterFloat> (
                info.id, info.name,
                juce::NormalisableRange<float> (info.minValue, info.maxValue, 0.01f, 0.5f),
                info.defaultValue));
        }
        else
        {
            params.push_back (std::make_unique<juce::AudioParameterFloat> (
                info.id, info.name,
                juce::NormalisableRange<float> (info.minValue, info.maxValue),
                info.defaultValue));
        }
    }

    params.push_back (std::make_unique<juce::AudioParameterInt> (
        "bars", "Bars", 1, 32, 1));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        "stepsPerBar", "Steps Per Bar", juce::StringArray ("4", "8", "16"), 2));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        "interpolation", "Interpolation", juce::StringArray ("Hold", "Glide"), 0));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "swing", "Swing",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
        0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "inputGain", "Input Gain",
        juce::NormalisableRange<float> (-48.0f, 12.0f, 0.1f),
        0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "outputGain", "Output Gain",
        juce::NormalisableRange<float> (-48.0f, 12.0f, 0.1f),
        0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "dryWet", "Dry / Wet",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
        1.0f));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        "syncMode", "Sync Mode", juce::StringArray ("Host", "MIDI Note"), 0));

    return { params.begin(), params.end() };
}

void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (getTotalNumOutputChannels());

    filterEngine.prepare (spec);
    delayEngine.prepare (sampleRate, samplesPerBlock);
    reverbEngine.prepare (spec);
    distortionEngine.prepare (spec);
    chorusEngine.prepare (spec);
    compressorEngine.prepare (sampleRate, samplesPerBlock);
    sequencerEngine.prepare (sampleRate);

    auto* barsParam = apvts.getRawParameterValue ("bars");
    auto* spbParam = apvts.getRawParameterValue ("stepsPerBar");
    if (barsParam != nullptr && spbParam != nullptr)
    {
        int bars = static_cast<int> (*barsParam);
        int spb = 4 << static_cast<int> (*spbParam);
        sequencerState.setGrid (bars, spb);
    }
}

void PluginProcessor::releaseResources()
{
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    auto mainOut = layouts.getMainOutputChannelSet();
    if (mainOut != juce::AudioChannelSet::mono() && mainOut != juce::AudioChannelSet::stereo())
        return false;

    auto mainIn = layouts.getMainInputChannelSet();
   #if ! JucePlugin_IsSynth
    if (mainIn.isDisabled() || mainOut != mainIn)
        return false;
   #endif

    auto sidechainIn = layouts.getChannelSet (true, 1);
    if (sidechainIn.isDisabled())
        return true;

    return (sidechainIn == juce::AudioChannelSet::mono() || sidechainIn == juce::AudioChannelSet::stereo());
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    auto output = getBusBuffer (buffer, false, 0);

    // Input gain
    auto* inGainParam = apvts.getRawParameterValue ("inputGain");
    if (inGainParam != nullptr)
        output.applyGain (juce::Decibels::decibelsToGain (inGainParam->load()));

    auto sidechainBuffer = getBusBuffer (buffer, true, 1);
    bool hasSidechain = getBus (true, 1) != nullptr && getBus (true, 1)->isEnabled()
                        && sidechainBuffer.getNumChannels() > 0;

    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf (output);

    auto* barsParam = apvts.getRawParameterValue ("bars");
    auto* spbParam = apvts.getRawParameterValue ("stepsPerBar");
    if (barsParam != nullptr && spbParam != nullptr)
    {
        int bars = static_cast<int> (*barsParam);
        int spbIndex = static_cast<int> (*spbParam);
        int spb = (spbIndex == 0) ? 4 : (spbIndex == 1) ? 8 : 16;
        if (bars != sequencerState.getNumBars() || spb != sequencerState.getStepsPerBar())
            sequencerState.setGrid (bars, spb);
    }

    auto* interpParam = apvts.getRawParameterValue ("interpolation");
    if (interpParam != nullptr)
    {
        sequencerEngine.setInterpolation (
            *interpParam > 0.5f ? SequencerEngine::Interpolation::Glide
                                : SequencerEngine::Interpolation::Hold);
    }

    auto* swingParam = apvts.getRawParameterValue ("swing");
    if (swingParam != nullptr)
    {
        sequencerEngine.setSwing (*swingParam);
    }

    auto* syncParam = apvts.getRawParameterValue ("syncMode");
    bool midiSync = (syncParam != nullptr && *syncParam > 0.5f);
    sequencerEngine.setSyncMode (midiSync ? SequencerEngine::SyncMode::MidiNote
                                           : SequencerEngine::SyncMode::Host);

    if (midiSync)
    {
        for (const auto meta : midiMessages)
        {
            auto msg = meta.getMessage();
            if (msg.isNoteOn())
                ++midiStepCounter;
        }
        sequencerEngine.setMidiStepIndex (midiStepCounter);
    }
    else
    {
        if (auto* ph = getPlayHead())
        {
            auto pos = ph->getPosition();
            if (pos.hasValue() && pos->getPpqPosition().hasValue())
                sequencerEngine.setPlayheadPPQ (*pos->getPpqPosition());
        }
        else
        {
            sequencerEngine.setPlayheadPPQ (-1.0);
        }
    }

    sequencerEngine.processBlock (output.getNumSamples());

    filterEngine.setCutoff (sequencerEngine.getSmoothedValue (ParameterMatrix::FilterCutoff));
    filterEngine.setResonance (sequencerEngine.getSmoothedValue (ParameterMatrix::FilterResonance));
    filterEngine.setMix (sequencerEngine.getSmoothedValue (ParameterMatrix::FilterMix));

    delayEngine.setTimeMs (sequencerEngine.getSmoothedValue (ParameterMatrix::DelayTime));
    delayEngine.setFeedback (sequencerEngine.getSmoothedValue (ParameterMatrix::DelayFeedback));
    delayEngine.setMix (sequencerEngine.getSmoothedValue (ParameterMatrix::DelayMix));

    reverbEngine.setSize (sequencerEngine.getSmoothedValue (ParameterMatrix::ReverbSize));
    reverbEngine.setDamping (sequencerEngine.getSmoothedValue (ParameterMatrix::ReverbDamping));
    reverbEngine.setMix (sequencerEngine.getSmoothedValue (ParameterMatrix::ReverbMix));

    distortionEngine.setDrive (sequencerEngine.getSmoothedValue (ParameterMatrix::DistortionDrive));
    distortionEngine.setTone (sequencerEngine.getSmoothedValue (ParameterMatrix::DistortionTone));
    distortionEngine.setMix (sequencerEngine.getSmoothedValue (ParameterMatrix::DistortionMix));

    chorusEngine.setRate (sequencerEngine.getSmoothedValue (ParameterMatrix::ChorusRate));
    chorusEngine.setDepth (sequencerEngine.getSmoothedValue (ParameterMatrix::ChorusDepth));
    chorusEngine.setMix (sequencerEngine.getSmoothedValue (ParameterMatrix::ChorusMix));

    compressorEngine.setThreshold (sequencerEngine.getSmoothedValue (ParameterMatrix::CompressorThreshold));
    compressorEngine.setRatio (sequencerEngine.getSmoothedValue (ParameterMatrix::CompressorRatio));
    compressorEngine.setMix (sequencerEngine.getSmoothedValue (ParameterMatrix::CompressorMix));

    bool anySolo = sequencerState.anySoloActive();
    const auto& order = sequencerState.getEffectOrder();

    for (int pos = 0; pos < ParameterMatrix::NumEffects; ++pos)
    {
        int effect = order[static_cast<size_t> (pos)];
        if (sequencerState.isBypassed (effect))
            continue;
        if (anySolo && ! sequencerState.isSoloed (effect))
            continue;

        processEffect (effect, output,
                       hasSidechain ? sidechainBuffer : output);
    }

    // Output gain
    auto* outGainParam = apvts.getRawParameterValue ("outputGain");
    if (outGainParam != nullptr)
        output.applyGain (juce::Decibels::decibelsToGain (outGainParam->load()));

    // Dry/Wet
    auto* dryWetParam = apvts.getRawParameterValue ("dryWet");
    if (dryWetParam != nullptr)
    {
        float wet = dryWetParam->load();
        float dryAmt = 1.0f - wet;
        for (int ch = 0; ch < output.getNumChannels(); ++ch)
        {
            auto* out = output.getWritePointer (ch);
            auto* dryPtr = dryBuffer.getReadPointer (ch);
            for (int s = 0; s < output.getNumSamples(); ++s)
                out[s] = dryPtr[s] * dryAmt + out[s] * wet;
        }
    }
}

void PluginProcessor::processEffect (int effectIndex, juce::AudioBuffer<float>& buffer,
                                     const juce::AudioBuffer<float>& sidechainBuffer)
{
    switch (effectIndex)
    {
        case ParameterMatrix::FilterEffect:      filterEngine.process (buffer); break;
        case ParameterMatrix::DelayEffect:       delayEngine.process (buffer); break;
        case ParameterMatrix::ReverbEffect:      reverbEngine.process (buffer); break;
        case ParameterMatrix::DistortionEffect:  distortionEngine.process (buffer); break;
        case ParameterMatrix::ChorusEffect:      chorusEngine.process (buffer); break;
        case ParameterMatrix::CompressorEffect:  compressorEngine.process (buffer, sidechainBuffer); break;
        default: break;
    }
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

bool PluginProcessor::hasEditor() const { return true; }

const juce::String PluginProcessor::getName() const { return JucePlugin_Name; }
bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}
bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}
bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}
double PluginProcessor::getTailLengthSeconds() const { return 3.0; }
int PluginProcessor::getNumPrograms() { return 1; }
int PluginProcessor::getCurrentProgram() { return 0; }
void PluginProcessor::setCurrentProgram (int) {}
const juce::String PluginProcessor::getProgramName (int) { return {}; }
void PluginProcessor::changeProgramName (int, const juce::String&) {}

void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto stateTree = apvts.copyState();

    juce::MemoryBlock seqBlock;
    sequencerState.writeToMemoryBlock (seqBlock);
    stateTree.setProperty ("sequencer_data", juce::Base64::toBase64 (seqBlock.getData(), seqBlock.getSize()), nullptr);

    std::unique_ptr<juce::XmlElement> xml (stateTree.createXml());
    copyXmlToBinary (*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr)
    {
        if (xml->hasTagName (apvts.state.getType()))
        {
            apvts.replaceState (juce::ValueTree::fromXml (*xml));

            auto seqData = apvts.state.getProperty ("sequencer_data").toString();
            if (seqData.isNotEmpty())
            {
                juce::MemoryBlock seqBlock;
                juce::MemoryOutputStream mos (seqBlock, false);
                if (juce::Base64::convertFromBase64 (mos, seqData))
                    sequencerState.readFromMemoryBlock (seqBlock);
            }
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

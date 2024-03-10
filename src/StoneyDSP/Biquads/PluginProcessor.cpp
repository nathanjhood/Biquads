/**
 * @file PluginProcessor.cpp
 * @author StoneyDSP (nathanjhood@googlemail.com)
 * @brief
 * @version 0.1
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "StoneyDSP/Biquads/PluginProcessor.hpp"
// #include "StoneyDSP/Biquads/PluginEditor.hpp"

namespace StoneyDSP {
namespace Biquads {

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
 #if ! JucePlugin_IsSynth
    .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
 #endif
    .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
)
, undoManager()
, apvts(*this, &undoManager, "Parameters", createParameterLayout())
, spec ()
// , outputPtr   (dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("outputID")))
// , coeffFlt(0.5f)
// , coeffDbl(0.5)
, processorFlt (*this)
, processorDbl (*this)
, bypassState (dynamic_cast<juce::AudioParameterBool*> (apvts.getParameter("bypassID")))
// , processingPrecision(singlePrecision)
{
    jassert(bypassState != nullptr);
    // jassert(outputPtr != nullptr);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorParameter* AudioPluginAudioProcessor::getBypassParameter() const
{
    return bypassState;
}

bool AudioPluginAudioProcessor::isBypassed() const noexcept
{
    return bypassState->get() == true;
}

void AudioPluginAudioProcessor::setBypassParameter(juce::AudioParameterBool* newBypass) noexcept
{
    if (bypassState != newBypass)
    {
        bypassState = newBypass;
        releaseResources();
        reset();
    }
}
//==============================================================================
bool AudioPluginAudioProcessor::supportsDoublePrecisionProcessing() const
{
    if (isUsingDoublePrecision() == true)
        return true;

    return false;
}

juce::AudioProcessor::ProcessingPrecision AudioPluginAudioProcessor::getProcessingPrecision() const noexcept
{
    return processingPrecision;
}

bool AudioPluginAudioProcessor::isUsingDoublePrecision() const noexcept
{
    return processingPrecision == doublePrecision;
}

void AudioPluginAudioProcessor::setProcessingPrecision(juce::AudioProcessor::ProcessingPrecision newPrecision) noexcept
{
    // If you hit this assertion then you're trying to use double precision
    // processing on a processor which does not support it!
    jassert(newPrecision != doublePrecision || supportsDoublePrecisionProcessing());

    if (processingPrecision != newPrecision)
    {
        processingPrecision = newPrecision;
        releaseResources();
        reset();
    }
}
//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    processingPrecision = getProcessingPrecision();

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    processorFlt.prepare(getSpec());
    processorDbl.prepare(getSpec());
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    processorFlt.reset();
    processorDbl.reset();
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // juce::ignoreUnused(midiMessages); // let them pass...

    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        juce::ignoreUnused (channelData);
        // ..do something to the data...
    }

    processorFlt.process(buffer, midiMessages);
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    // juce::ignoreUnused(midiMessages); // let them pass...

    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        juce::ignoreUnused (channelData);
        // ..do something to the data...
    }

    processorDbl.process(buffer, midiMessages);
}

void AudioPluginAudioProcessor::processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    processorFlt.processBypass(buffer, midiMessages);
}

void AudioPluginAudioProcessor::processBlockBypassed(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    processorDbl.processBypass(buffer, midiMessages);
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    // return new AudioPluginAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this);
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout parameterLayout;

    // const auto dBMax = juce::Decibels::gainToDecibels(16.0f);
    // const auto dBMin = juce::Decibels::gainToDecibels(0.0625f);
    // const auto dBOut = juce::Decibels::gainToDecibels(0.5f, -120.0f); //  * 20.0f

    // constexpr auto minusInfinityDb = 120.0f;

    // const auto outputRangeDBMin = juce::Decibels::gainToDecibels(00.0f, minusInfinityDb);
    // const auto outputRangeDBMax = juce::Decibels::gainToDecibels(16.0f, minusInfinityDb);

    // const auto outputRangeMin = juce::Decibels::decibelsToGain(outputRangeDBMin, minusInfinityDb);
    // const auto outputRangeMax = juce::Decibels::decibelsToGain(outputRangeDBMax, minusInfinityDb);

    // const auto freqRange = juce::NormalisableRange<float>(20.00f, 20000.00f, 0.001f, 00.198894f);
    // const auto resRange = juce::NormalisableRange<float>(00.00f, 1.00f, 0.01f, 1.00f);
    // const auto gainRange = juce::NormalisableRange<float>(dBMin, dBMax, 0.01f, 1.00f);
    // const auto mixRange = juce::NormalisableRange<float>(00.00f, 100.00f, 0.01f, 1.00f);
    // const auto outputRange = juce::NormalisableRange<float>(outputRangeMin, outputRangeMax, 0.01f, 1.00f);

    // const auto fString = juce::StringArray({ "LP2", "LP1", "HP2", "HP1" , "BP2", "BP2c", "LS2", "LS1c", "LS1", "HS2", "HS1c", "HS1", "PK2", "NX2", "AP2" });
    // const auto tString = juce::StringArray({ "DFI", "DFII", "DFI t", "DFII t" });
    // const auto osString = juce::StringArray({ "--", "2x", "4x", "8x", "16x" });

    // const auto decibels = juce::String{ ("dB") };
    // const auto frequency = juce::String{ ("Hz") };
    // const auto reso = juce::String{ ("q") };
    // const auto percentage = juce::String{ ("%") };

    // auto genParam = juce::AudioProcessorParameter::genericParameter;
    // auto inMeter = juce::AudioProcessorParameter::inputMeter;
    // auto outParam = juce::AudioProcessorParameter::outputGain;
    // auto outMeter = juce::AudioProcessorParameter::outputMeter;

    // auto mixAttributes = juce::AudioParameterFloatAttributes()
    //     .withLabel(percentage)
    //     .withCategory(genParam);

    // auto outputAttributes = juce::AudioParameterFloatAttributes()
    //     .withLabel(decibels)
    //     .withCategory(outParam);

    // parameterLayout.add
    //     //======================================================================
    //     (std::make_unique<juce::AudioProcessorParameterGroup>("masterID", "0", "seperatorA",
    //         //==================================================================
    //         std::make_unique<juce::AudioParameterFloat>("outputID", "Output", outputRange, 00.00f, outputAttributes),
    //         // std::make_unique<juce::AudioParameterFloat>("mixID", "Mix", mixRange, 100.00f, mixAttributes)
    //         //==================================================================
    //     ));

    parameterLayout.add(std::make_unique<juce::AudioParameterBool>("bypassID", "Bypass", false));
    // parameterLayout.add(std::make_unique<juce::AudioParameterFloat>("outputID", "Output", outputRange, 00.00f, outputAttributes));

    // Parameters::setParameterLayout(parameterLayout);

    return parameterLayout;
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AudioPluginAudioProcessor::getCurrentProgramStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void AudioPluginAudioProcessor::setCurrentProgramStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

} // namespace Biquads
} // namespace StoneyDSP

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StoneyDSP::Biquads::AudioPluginAudioProcessor();
}

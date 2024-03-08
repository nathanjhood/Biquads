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
#include "StoneyDSP/Biquads/PluginEditor.hpp"

//==============================================================================
BiquadsAudioProcessor::BiquadsAudioProcessor()
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
// , processorFloat (*this)
// , processorDouble (*this)
, bypassState (dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("bypassID")))
{
    jassert(bypassState != nullptr);
}

BiquadsAudioProcessor::~BiquadsAudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorParameter* BiquadsAudioProcessor::getBypassParameter() const
{
    return bypassState;
}

bool BiquadsAudioProcessor::isBypassed() const noexcept
{
    return bypassState->get() == true;
}

void BiquadsAudioProcessor::setBypassParameter(juce::AudioParameterBool* newBypass) noexcept
{
    if (bypassState != newBypass)
    {
        bypassState = newBypass;
        releaseResources();
        reset();
    }
}

//==============================================================================
const juce::String BiquadsAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BiquadsAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BiquadsAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BiquadsAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BiquadsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BiquadsAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BiquadsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BiquadsAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String BiquadsAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void BiquadsAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void BiquadsAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);

    // processorFloat.prepare( getSpec() );
    // processorDouble.prepare( getSpec() );
}

void BiquadsAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool BiquadsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void BiquadsAudioProcessor::processBlock (
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages
)
{
    juce::ignoreUnused(midiMessages);
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

    // switch(bypassState->get())
    // {
    //     case false:

    //         processorFloat.process(buffer, midiMessages);

    //     default:

    //         processBlockBypassed(buffer, midiMessages);
    // }
}

void BiquadsAudioProcessor::processBlock (
    juce::AudioBuffer<double>& buffer,
    juce::MidiBuffer& midiMessages
)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        juce::ignoreUnused (channelData);
        // ..do something to the data...
    }

    // switch(bypassState->get())
    // {
    //     case false:

    //         processorDouble.process(buffer, midiMessages);

    //     default:

    //         processBlockBypassed(buffer, midiMessages);
    // }
}

void BiquadsAudioProcessor::processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    midiMessages.clear();

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing context(block);

    const auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();

    outputBlock.copyFrom(inputBlock);
}

void BiquadsAudioProcessor::processBlockBypassed(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    midiMessages.clear();

    juce::dsp::AudioBlock<double> block(buffer);
    juce::dsp::ProcessContextReplacing context(block);

    const auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();

    outputBlock.copyFrom(inputBlock);
}

//==============================================================================
bool BiquadsAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BiquadsAudioProcessor::createEditor()
{
    return new BiquadsAudioProcessorEditor (*this);
}

juce::AudioProcessorValueTreeState::ParameterLayout BiquadsAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout parameterLayout;

    parameterLayout.add(std::make_unique<juce::AudioParameterBool>("bypassID", "Bypass", false));

    // Parameters::setParameterLayout(parameterLayout);

    return parameterLayout;
}

//==============================================================================
void BiquadsAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void BiquadsAudioProcessor::getCurrentProgramStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void BiquadsAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void BiquadsAudioProcessor::setCurrentProgramStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BiquadsAudioProcessor();
}

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BiquadsAudioProcessor::BiquadsAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       ), 
    apvts( *this, &undoManager, "Parameters", createParameterLayout() )
{
    bypassPtr = static_cast <juce::AudioParameterBool*> (apvts.getParameter("bypassID"));
    jassert(bypassPtr != nullptr);
}

BiquadsAudioProcessor::~BiquadsAudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorParameter* BiquadsAudioProcessor::getBypassParameter() const
{
    return bypassPtr;
}

bool BiquadsAudioProcessor::supportsDoublePrecisionProcessing() const
{
    return true;
}

juce::AudioProcessor::ProcessingPrecision BiquadsAudioProcessor::getProcessingPrecision() const noexcept
{
    return processingPrecision;
}

bool BiquadsAudioProcessor::isUsingDoublePrecision() const noexcept
{ 
    return processingPrecision == doublePrecision; 
}

void BiquadsAudioProcessor::setProcessingPrecision(ProcessingPrecision newPrecision) noexcept
{
    if (processingPrecision != newPrecision)
    {
        processingPrecision = newPrecision;
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
    return false;
}

bool BiquadsAudioProcessor::producesMidi() const
{
    return false;
}

bool BiquadsAudioProcessor::isMidiEffect() const
{
    return false;
}

double BiquadsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BiquadsAudioProcessor::getNumPrograms()
{
    return 127;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BiquadsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BiquadsAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String BiquadsAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void BiquadsAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index);
    juce::ignoreUnused(newName);
}

//==============================================================================
void BiquadsAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    getProcessingPrecision();

    processorFloat.prepare(sampleRate, samplesPerBlock);
    processorDouble.prepare(sampleRate, samplesPerBlock);
}

void BiquadsAudioProcessor::releaseResources()
{
    processorFloat.reset();
    processorDouble.reset();
}

void BiquadsAudioProcessor::numChannelsChanged()
{
    releaseResources();
}

void BiquadsAudioProcessor::numBusesChanged()
{
    releaseResources();
}

void BiquadsAudioProcessor::processorLayoutsChanged()
{
    releaseResources();
}

bool BiquadsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void BiquadsAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (bypassPtr->get() == true)
    {
        processBlockBypassed(buffer, midiMessages);
    }

    else
    {
        juce::ScopedNoDenormals noDenormals;

        processorFloat.process(buffer, midiMessages);
    }
}

void BiquadsAudioProcessor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    if (bypassPtr->get() == true)
    {
        processBlockBypassed(buffer, midiMessages);
    }

    else
    {
        juce::ScopedNoDenormals noDenormals;

        processorDouble.process(buffer, midiMessages);
    }
}

void BiquadsAudioProcessor::processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer);
    juce::ignoreUnused(midiMessages);
}

void BiquadsAudioProcessor::processBlockBypassed(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer);
    juce::ignoreUnused(midiMessages);
}

//==============================================================================
bool BiquadsAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* BiquadsAudioProcessor::createEditor()
{
    return new BiquadsAudioProcessorEditor(*this, getAPVTS(), undoManager);
}

juce::AudioProcessorValueTreeState::ParameterLayout BiquadsAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout params;

    Parameters::setParameterLayout(params);

    return params;
}

//==============================================================================
void BiquadsAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void BiquadsAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BiquadsAudioProcessor();
}

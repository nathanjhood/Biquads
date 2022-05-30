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
                       )
{
    doublesPtr = dynamic_cast       <juce::AudioParameterChoice*>    (apvts.getParameter("precisionID"));
    jassert(doublesPtr != nullptr);

    bypassPtr = dynamic_cast       <juce::AudioParameterBool*>    (apvts.getParameter("bypassID"));
    jassert(bypassPtr != nullptr);
}

BiquadsAudioProcessor::~BiquadsAudioProcessor()
{
}

juce::AudioProcessorValueTreeState& BiquadsAudioProcessor::getAPVTS()
{
    return apvts;
}

juce::AudioProcessorValueTreeState::ParameterLayout BiquadsAudioProcessor::getParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    Parameters::setParameterLayout(params);

    auto pString = juce::StringArray({ "Floats", "Doubles" });

    params.push_back(std::make_unique<juce::AudioParameterChoice>("precisionID", "Precision", pString, 0));
    params.push_back(std::make_unique<juce::AudioParameterBool>("bypassID", "Bypass", false));

    return { params.begin(), params.end() };
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

void BiquadsAudioProcessor::setProcessingPrecision(ProcessingPrecision newPrecision) noexcept
{
    if (processingPrecision != newPrecision)
    {
        processingPrecision = newPrecision;
        releaseResources();
        reset();
    }
}

juce::AudioProcessor::ProcessingPrecision BiquadsAudioProcessor::getProcessingPrecision() const noexcept
{ 
    return processingPrecision;
    //return static_cast<ProcessingPrecision>(doublesPtr->getIndex());
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
    processorFloat.reset();
    processorDouble.reset();
}

bool BiquadsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void BiquadsAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (doublesPtr->getIndex() == 0)

        if (bypassPtr->get() == false)

        {
            juce::ScopedNoDenormals noDenormals;

            processorFloat.process(buffer, midiMessages);
        }

        else
        {
            processBlockBypassed(buffer, midiMessages);
        }
}

void BiquadsAudioProcessor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    if (doublesPtr->getIndex() == 1)

        if (bypassPtr->get() == false)

        {
            juce::ScopedNoDenormals noDenormals;

            processorDouble.process(buffer, midiMessages);
        }

        else
        {
            processBlockBypassed(buffer, midiMessages);
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
    //return new BiquadsAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
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

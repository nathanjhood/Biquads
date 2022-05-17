/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BiquadsAudioProcessor::BiquadsAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    doublesPtr = dynamic_cast       <juce::AudioParameterBool*>    (apvts.getParameter("doublesID"));
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

juce::AudioProcessorValueTreeState::ParameterLayout BiquadsAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    ProcessWrapper<float>::createParameterLayout(params);

    params.push_back(std::make_unique<juce::AudioParameterBool>("doublesID", "Doubles", false));
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
    return false;
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
    auto precision = getProcessingPrecision();
    auto numChannels = getTotalNumOutputChannels();

    processor.prepare(sampleRate, samplesPerBlock, numChannels);
}

void BiquadsAudioProcessor::releaseResources()
{
    processor.reset();
}

void BiquadsAudioProcessor::update()
{
    bypassPtr->get();
    doublesPtr->get();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BiquadsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BiquadsAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (bypassPtr->get() == false)

    {
        juce::ScopedNoDenormals noDenormals;

        update();

        processor.process(buffer, midiMessages);
    }

    else
    {
        processBlockBypassed(buffer, midiMessages);
    }
}

void BiquadsAudioProcessor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    if (bypassPtr->get() == false)

    {
        juce::ScopedNoDenormals noDenormals;

        update();

        juce::ignoreUnused(buffer);
        juce::ignoreUnused(midiMessages);
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

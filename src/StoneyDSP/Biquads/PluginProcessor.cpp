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

#include "StoneyDSP/Biquads/PluginProcessor.h"
#include "StoneyDSP/Biquads/PluginEditor.h"

//==============================================================================
BiquadsAudioProcessor::BiquadsAudioProcessor()
     :
    AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
    undoManager (),
    apvts (*this, &undoManager, "Parameters", createParameterLayout()),
    spec (),
    rmsLeft(), rmsRight(),
    parameters (*this),
    processorFloat (*this),
    processorDouble (*this),
    bypassState (dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("bypassID"))),
    processingPrecision(singlePrecision)
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

bool BiquadsAudioProcessor::supportsDoublePrecisionProcessing() const
{
    if (isUsingDoublePrecision() == true)
        return true;

    return false;
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
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void BiquadsAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);

    processingPrecision = getProcessingPrecision();

    spec.sampleRate = getSampleRate();
    spec.maximumBlockSize = getBlockSize();
    spec.numChannels = getTotalNumInputChannels();

    rmsLeft.reset(sampleRate, rampDurationSeconds);
    rmsRight.reset(sampleRate, rampDurationSeconds);

    rmsLeft.setCurrentAndTargetValue(-100.0f);
    rmsRight.setCurrentAndTargetValue(-100.0f);

    processorFloat.prepare( getSpec() );
    processorDouble.prepare( getSpec() );
}

void BiquadsAudioProcessor::releaseResources()
{
    processorFloat.reset();
    processorDouble.reset();
}

bool BiquadsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void BiquadsAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (bypassState->get())
    {
        processBlockBypassed(buffer, midiMessages);
    }

    else
    {
        juce::ScopedNoDenormals noDenormals;

        processorFloat.process(buffer, midiMessages);

        rmsLeft.skip(buffer.getNumSamples());
        rmsRight.skip(buffer.getNumSamples());

        {
            const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
            if (value < rmsLeft.getCurrentValue())
                rmsLeft.setTargetValue(value);
            else
                rmsLeft.setCurrentAndTargetValue(value);
        }

        {
            const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
            if (value < rmsRight.getCurrentValue())
                rmsRight.setTargetValue(value);
            else
                rmsRight.setCurrentAndTargetValue(value);
        }
    }
}

void BiquadsAudioProcessor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    if (bypassState->get())
    {
        processBlockBypassed(buffer, midiMessages);
    }

    else
    {
        juce::ScopedNoDenormals noDenormals;

        processorDouble.process(buffer, midiMessages);

        rmsLeft.skip(buffer.getNumSamples());
        rmsRight.skip(buffer.getNumSamples());

        {
            const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
            if (value < rmsLeft.getCurrentValue())
                rmsLeft.setTargetValue(value);
            else
                rmsLeft.setCurrentAndTargetValue(value);
        }

        {
            const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
            if (value < rmsRight.getCurrentValue())
                rmsRight.setTargetValue(value);
            else
                rmsRight.setCurrentAndTargetValue(value);
        }
    }
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
    return true;
}

juce::AudioProcessorEditor* BiquadsAudioProcessor::createEditor()
{
    return new BiquadsAudioProcessorEditor(*this);
}

juce::AudioProcessorValueTreeState::ParameterLayout BiquadsAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout params;

    params.add(std::make_unique<juce::AudioParameterBool>("bypassID", "Bypass", false));

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

void BiquadsAudioProcessor::getCurrentProgramStateInformation(juce::MemoryBlock& destData)
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

void BiquadsAudioProcessor::setCurrentProgramStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

float BiquadsAudioProcessor::getRMSLevel(const int channel) const
{
    jassert(channel == 0 || channel == 1);
    if (channel == 0)
        return rmsLeft.getCurrentValue();
    if (channel == 1)
        return rmsRight.getCurrentValue();
    return 0.0f;
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BiquadsAudioProcessor();
}

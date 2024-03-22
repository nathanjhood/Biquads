/***************************************************************************//**
 * @file Processor.cpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief Simple two-pole equalizer with variable oversampling.
 * @version 1.2.3.155
 * @date 2024-03-16
 *
 * @copyright Copyright (c) 2024 - Nathan J. Hood

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

 ******************************************************************************/

// #ifdef STONEYDSP_BIQUADS_PROCESSOR_HPP_INCLUDED
//  #error "Incorrect usage of 'Processor.cpp'!"
// #endif

#include "Processor.hpp"

namespace StoneyDSP {
/** @addtogroup StoneyDSP @{ */

namespace Biquads {
/** @addtogroup Biquads @{ */

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
  , spec()
  , parametersPtr(std::make_unique<AudioPluginAudioProcessorParameters>(*this))
  , parameters(*parametersPtr.get())
  , undoManager(parameters.getUndoManager())
  , apvts(parameters.getApvts())
  , processorFltPtr(std::make_unique<AudioPluginAudioProcessorWrapper<float>>(*this, parameters.getApvts(), getSpec()))
  , processorDblPtr(std::make_unique<AudioPluginAudioProcessorWrapper<double>>(*this, parameters.getApvts(), getSpec()))
  , processorFlt(*processorFltPtr.get())
  , processorDbl(*processorDblPtr.get())
  , bypassState(dynamic_cast <juce::AudioParameterBool*>(parameters.getApvts().getParameter("Master_bypassID")))
{
    bypassState = dynamic_cast <juce::AudioParameterBool*>(parameters.getApvts().getParameter("Master_bypassID"));

    jassert(parametersPtr       != nullptr);
    jassert(processorFltPtr     != nullptr);
    jassert(processorFltPtr     != nullptr);

    jassert(bypassState         != nullptr);
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
    return ProjectInfo::projectName;
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
    AudioPluginAudioProcessorWrapper<float>&  processorFloat = *processorFltPtr.get();
    AudioPluginAudioProcessorWrapper<double>& processorDouble = *processorDblPtr.get();

    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    processingPrecision = getProcessingPrecision();

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    if(!isUsingDoublePrecision())
    {
        processorFloat.prepare(getSpec());
        processorDouble.reset(0.0);
    }
    else
    {
        processorFloat.reset(0.0f);
        processorDouble.prepare(getSpec());
    }
}

void AudioPluginAudioProcessor::releaseResources()
{
    AudioPluginAudioProcessorWrapper<float>&  processorFloat  = getProcessorFlt();
    AudioPluginAudioProcessorWrapper<double>& processorDouble = getProcessorDbl();

    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    if(!isUsingDoublePrecision())
    {
        processorFloat.reset(0.0f);
    }
    else
    {
        processorDouble.reset(0.0);
    }
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
    if(!isBypassed())
    {
        jassert (! isUsingDoublePrecision());

        AudioPluginAudioProcessorWrapper<float>& processor = getProcessorFlt();

        juce::ScopedNoDenormals noDenormals;

        processor.process(buffer, midiMessages);
    }
    else
    {
        processBlockBypassed(buffer, midiMessages);
    }
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    if(!isBypassed())
    {
        jassert (isUsingDoublePrecision());

        AudioPluginAudioProcessorWrapper<double>& processor = getProcessorDbl();

        juce::ScopedNoDenormals noDenormals;

        processor.process(buffer, midiMessages);
    }
    else
    {
        processBlockBypassed(buffer, midiMessages);
    }
}

void AudioPluginAudioProcessor::processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    jassert (! isUsingDoublePrecision());

    AudioPluginAudioProcessorWrapper<float>& processor = getProcessorFlt();

    processor.processBypass(buffer, midiMessages);
}

void AudioPluginAudioProcessor::processBlockBypassed(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    jassert (isUsingDoublePrecision());

    AudioPluginAudioProcessorWrapper<double>& processor = getProcessorDbl();

    processor.processBypass(buffer, midiMessages);
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

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto& valueTreeState = parameters.getApvts();
    auto state = valueTreeState.copyState();

    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AudioPluginAudioProcessor::getCurrentProgramStateInformation(juce::MemoryBlock& destData)
{
    auto& valueTreeState = parameters.getApvts();
    auto state = valueTreeState.copyState();

    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml.get(), destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto& valueTreeState = parameters.getApvts();

    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(valueTreeState.state.getType()))
            valueTreeState.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void AudioPluginAudioProcessor::setCurrentProgramStateInformation(const void* data, int sizeInBytes)
{
    auto& valueTreeState = parameters.getApvts();

    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(valueTreeState.state.getType()))
            valueTreeState.replaceState(juce::ValueTree::fromXml(*xmlState));
}

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP

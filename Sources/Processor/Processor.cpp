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
Processor::Processor()
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
 #if ! JucePlugin_IsSynth
    .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
 #endif
    .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
)
, spec()
// , undoManagerPtr  (std::make_unique<juce::UndoManager>())
// , undoManager     (*undoManagerPtr.get())
// , apvtsPtr        (std::make_unique<juce::AudioProcessorValueTreeState>(*this, getUndoManager(), juce::Identifier { "Parameters" }, getParameterLayout() ))
// , apvts           (*apvtsPtr.get())
, undoManager()
, apvts(*this, &undoManager, juce::Identifier { "Parameters" }, createParameterLayout())
, bypassState(dynamic_cast <juce::AudioParameterBool*>(getApvts().getParameter("Master_bypassID")))
//   , parametersPtr(std::make_unique<ProcessorParameters>(*this))
//   , parameters(*parametersPtr.get())
//   , undoManager(parameters.getUndoManager())
//   , apvts(parameters.getApvts())
//   , processorFltPtr(std::make_unique<ProcessorWrapper<float>>(*this, parameters.getApvts(), getSpec()))
//   , processorDblPtr(std::make_unique<ProcessorWrapper<double>>(*this, parameters.getApvts(), getSpec()))
//   , processorFlt(*processorFltPtr.get())
//   , processorDbl(*processorDblPtr.get())
//   , bypassState(dynamic_cast <juce::AudioParameterBool*>(parameters.getApvts().getParameter("Master_bypassID")))
{
    bypassState = dynamic_cast <juce::AudioParameterBool*>(getApvts().getParameter("Master_bypassID"));

    // jassert(parametersPtr       != nullptr);
    // jassert(processorFltPtr     != nullptr);
    // jassert(processorFltPtr     != nullptr);

    // jassert(undoManagerPtr != nullptr);
    // jassert(parameterLayoutPtr != nullptr);
    // jassert(apvtsPtr != nullptr);

    jassert(bypassState != nullptr);
}

Processor::~Processor()
{
}

//==============================================================================
juce::AudioProcessorParameter* Processor::getBypassParameter() const
{
    return bypassState;
}

bool Processor::isBypassed() const noexcept
{
    return bypassState->get() == true;
}

void Processor::setBypassParameter(juce::AudioParameterBool* newBypass) noexcept
{
    if (bypassState != newBypass)
    {
        bypassState = newBypass;
        releaseResources();
        reset();
    }
}
//==============================================================================
bool Processor::supportsDoublePrecisionProcessing() const
{
    if (isUsingDoublePrecision() == true)
        return true;

    return false;
}

juce::AudioProcessor::ProcessingPrecision Processor::getProcessingPrecision() const noexcept
{
    return processingPrecision;
}

bool Processor::isUsingDoublePrecision() const noexcept
{
    return processingPrecision == doublePrecision;
}

void Processor::setProcessingPrecision(juce::AudioProcessor::ProcessingPrecision newPrecision) noexcept
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
const juce::String Processor::getName() const
{
    return ProjectInfo::projectName;
}

bool Processor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Processor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Processor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Processor::getTailLengthSeconds() const
{
    return 0.0;
}

int Processor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Processor::getCurrentProgram()
{
    return 0;
}

void Processor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String Processor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void Processor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void Processor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // ProcessorWrapper<float>&  processorFloat = *processorFltPtr.get();
    // ProcessorWrapper<double>& processorDouble = *processorDblPtr.get();

    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    processingPrecision = getProcessingPrecision();

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    // if(!isUsingDoublePrecision())
    // {
    //     processorFloat.prepare(getSpec());
    //     processorDouble.reset(0.0);
    // }
    // else
    // {
    //     processorFloat.reset(0.0f);
    //     processorDouble.prepare(getSpec());
    // }
}

void Processor::releaseResources()
{
    // ProcessorWrapper<float>&  processorFloat  = getProcessorFlt();
    // ProcessorWrapper<double>& processorDouble = getProcessorDbl();

    // // When playback stops, you can use this as an opportunity to free up any
    // // spare memory, etc.
    // if(!isUsingDoublePrecision())
    // {
    //     processorFloat.reset(0.0f);
    // }
    // else
    // {
    //     processorDouble.reset(0.0);
    // }
}

bool Processor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void Processor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // if(!isBypassed())
    // {
        jassert (! isUsingDoublePrecision());

    //     ProcessorWrapper<float>& processor = getProcessorFlt();

        juce::ScopedNoDenormals noDenormals;

    //     processor.process(buffer, midiMessages);
    // }
    // else
    // {
        processBlockBypassed(buffer, midiMessages);
    // }
}

void Processor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    // if(!isBypassed())
    // {
        jassert (isUsingDoublePrecision());

    //     ProcessorWrapper<double>& processor = getProcessorDbl();

        juce::ScopedNoDenormals noDenormals;

    //     processor.process(buffer, midiMessages);
    // }
    // else
    // {
        processBlockBypassed(buffer, midiMessages);
    // }
}

void Processor::processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    jassert (! isUsingDoublePrecision());

    juce::ignoreUnused(buffer, midiMessages);

    // ProcessorWrapper<float>& processor = getProcessorFlt();

    // processor.processBypass(buffer, midiMessages);
}

void Processor::processBlockBypassed(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    jassert (isUsingDoublePrecision());

    juce::ignoreUnused(buffer, midiMessages);

    // ProcessorWrapper<double>& processor = getProcessorDbl();

    // processor.processBypass(buffer, midiMessages);
}

//==============================================================================
bool Processor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Processor::createEditor()
{
    // return new ProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void Processor::getStateInformation (juce::MemoryBlock& destData)
{
    auto& valueTreeState = getApvts();
    auto state = valueTreeState.copyState();

    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Processor::getCurrentProgramStateInformation(juce::MemoryBlock& destData)
{
    auto& valueTreeState = getApvts();
    auto state = valueTreeState.copyState();

    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml.get(), destData);
}

void Processor::setStateInformation (const void* data, int sizeInBytes)
{
    auto& valueTreeState = getApvts();

    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(valueTreeState.state.getType()))
            valueTreeState.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void Processor::setCurrentProgramStateInformation(const void* data, int sizeInBytes)
{
    auto& valueTreeState = getApvts();

    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(valueTreeState.state.getType()))
            valueTreeState.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout Processor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout parameterLayout;

    // setParameterLayout(parameterLayout);

    return parameterLayout;
}

  /** @} group Biquads */
} // namespace Biquads

  /** @} group StoneyDSP */
} // namespace StoneyDSP

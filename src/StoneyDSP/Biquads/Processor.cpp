/***************************************************************************//**
 * @file Processor.cpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief Simple two-pole equalizer with variable oversampling.
 * @version 1.2.2.151
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

#include "StoneyDSP/Biquads.hpp"

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
, undoManager()
, apvts(*this, &undoManager, "Parameters", createParameterLayout())
, spec()
, parameters(*this, getAPVTS())
, processorFlt(*this, getAPVTS(), getSpec())
, processorDbl(*this, getAPVTS(), getSpec())

, masterBypassPtr (dynamic_cast<juce::AudioParameterBool*>          (apvts.getParameter("Master_bypassID")))
, masterOutputPtr (dynamic_cast <juce::AudioParameterFloat*>        (apvts.getParameter("Master_outputID")))
, masterMixPtr (dynamic_cast <juce::AudioParameterFloat*>           (apvts.getParameter("Master_mixID")))
, masterOsPtr (dynamic_cast <juce::AudioParameterChoice*>           (apvts.getParameter("Master_osID")))
, masterTransformPtr (dynamic_cast <juce::AudioParameterChoice*>    (apvts.getParameter("Master_transformID")))

, biquadsABypassPtr (dynamic_cast<juce::AudioParameterBool*>        (apvts.getParameter("Band_A_bypassID")))
, biquadsAFrequencyPtr (dynamic_cast <juce::AudioParameterFloat*>   (apvts.getParameter("Band_A_frequencyID")))
, biquadsAResonancePtr (dynamic_cast <juce::AudioParameterFloat*>   (apvts.getParameter("Band_A_resonanceID")))
, biquadsAGainPtr (dynamic_cast <juce::AudioParameterFloat*>        (apvts.getParameter("Band_A_gainID")))
, biquadsATypePtr(dynamic_cast <juce::AudioParameterChoice*>        (apvts.getParameter("Band_A_typeID")))

, biquadsBBypassPtr (dynamic_cast<juce::AudioParameterBool*>        (apvts.getParameter("Band_B_bypassID")))
, biquadsBFrequencyPtr (dynamic_cast <juce::AudioParameterFloat*>   (apvts.getParameter("Band_B_frequencyID")))
, biquadsBResonancePtr (dynamic_cast <juce::AudioParameterFloat*>   (apvts.getParameter("Band_B_resonanceID")))
, biquadsBGainPtr (dynamic_cast <juce::AudioParameterFloat*>        (apvts.getParameter("Band_B_gainID")))
, biquadsBTypePtr(dynamic_cast <juce::AudioParameterChoice*>        (apvts.getParameter("Band_B_typeID")))

, biquadsCBypassPtr (dynamic_cast<juce::AudioParameterBool*>        (apvts.getParameter("Band_C_bypassID")))
, biquadsCFrequencyPtr (dynamic_cast <juce::AudioParameterFloat*>   (apvts.getParameter("Band_C_frequencyID")))
, biquadsCResonancePtr (dynamic_cast <juce::AudioParameterFloat*>   (apvts.getParameter("Band_C_resonanceID")))
, biquadsCGainPtr (dynamic_cast <juce::AudioParameterFloat*>        (apvts.getParameter("Band_C_gainID")))
, biquadsCTypePtr(dynamic_cast <juce::AudioParameterChoice*>        (apvts.getParameter("Band_C_typeID")))

, biquadsDBypassPtr (dynamic_cast<juce::AudioParameterBool*>        (apvts.getParameter("Band_D_bypassID")))
, biquadsDFrequencyPtr (dynamic_cast <juce::AudioParameterFloat*>   (apvts.getParameter("Band_D_frequencyID")))
, biquadsDResonancePtr (dynamic_cast <juce::AudioParameterFloat*>   (apvts.getParameter("Band_D_resonanceID")))
, biquadsDGainPtr (dynamic_cast <juce::AudioParameterFloat*>        (apvts.getParameter("Band_D_gainID")))
, biquadsDTypePtr(dynamic_cast <juce::AudioParameterChoice*>        (apvts.getParameter("Band_D_typeID")))

, bypassState (dynamic_cast<juce::AudioParameterBool*>              (apvts.getParameter("Master_bypassID")))
// , processingPrecision(singlePrecision)
{
    masterBypassPtr = dynamic_cast<juce::AudioParameterBool*>           (apvts.getParameter("Master_bypassID"));
    masterOutputPtr = dynamic_cast <juce::AudioParameterFloat*>         (apvts.getParameter("Master_outputID"));
    masterMixPtr = dynamic_cast <juce::AudioParameterFloat*>            (apvts.getParameter("Master_mixID"));
    masterOsPtr = dynamic_cast <juce::AudioParameterChoice*>            (apvts.getParameter("Master_osID"));
    masterTransformPtr = dynamic_cast <juce::AudioParameterChoice*>     (apvts.getParameter("Master_transformID"));

    biquadsABypassPtr = dynamic_cast<juce::AudioParameterBool*>         (apvts.getParameter("Band_A_bypassID"));
    biquadsAFrequencyPtr = dynamic_cast <juce::AudioParameterFloat*>    (apvts.getParameter("Band_A_frequencyID"));
    biquadsAResonancePtr = dynamic_cast <juce::AudioParameterFloat*>    (apvts.getParameter("Band_A_resonanceID"));
    biquadsAGainPtr = dynamic_cast <juce::AudioParameterFloat*>         (apvts.getParameter("Band_A_gainID"));
    biquadsATypePtr = dynamic_cast <juce::AudioParameterChoice*>        (apvts.getParameter("Band_A_typeID"));

    biquadsBBypassPtr = dynamic_cast<juce::AudioParameterBool*>         (apvts.getParameter("Band_B_bypassID"));
    biquadsBFrequencyPtr = dynamic_cast <juce::AudioParameterFloat*>    (apvts.getParameter("Band_B_frequencyID"));
    biquadsBResonancePtr = dynamic_cast <juce::AudioParameterFloat*>    (apvts.getParameter("Band_B_resonanceID"));
    biquadsBGainPtr = dynamic_cast <juce::AudioParameterFloat*>         (apvts.getParameter("Band_B_gainID"));
    biquadsBTypePtr = dynamic_cast <juce::AudioParameterChoice*>        (apvts.getParameter("Band_B_typeID"));

    biquadsCBypassPtr = dynamic_cast<juce::AudioParameterBool*>         (apvts.getParameter("Band_C_bypassID"));
    biquadsCFrequencyPtr = dynamic_cast <juce::AudioParameterFloat*>    (apvts.getParameter("Band_C_frequencyID"));
    biquadsCResonancePtr = dynamic_cast <juce::AudioParameterFloat*>    (apvts.getParameter("Band_C_resonanceID"));
    biquadsCGainPtr = dynamic_cast <juce::AudioParameterFloat*>         (apvts.getParameter("Band_C_gainID"));
    biquadsCTypePtr = dynamic_cast <juce::AudioParameterChoice*>        (apvts.getParameter("Band_C_typeID"));

    biquadsDBypassPtr = dynamic_cast<juce::AudioParameterBool*>         (apvts.getParameter("Band_D_bypassID"));
    biquadsDFrequencyPtr = dynamic_cast <juce::AudioParameterFloat*>    (apvts.getParameter("Band_D_frequencyID"));
    biquadsDResonancePtr = dynamic_cast <juce::AudioParameterFloat*>    (apvts.getParameter("Band_D_resonanceID"));
    biquadsDGainPtr = dynamic_cast <juce::AudioParameterFloat*>         (apvts.getParameter("Band_D_gainID"));
    biquadsDTypePtr = dynamic_cast <juce::AudioParameterChoice*>        (apvts.getParameter("Band_D_typeID"));

    bypassState = dynamic_cast <juce::AudioParameterBool*>              (apvts.getParameter("Master_bypassID"));

    jassert(masterBypassPtr             != nullptr);
    jassert(masterOutputPtr             != nullptr);
    jassert(masterMixPtr                != nullptr);
    jassert(masterOsPtr                 != nullptr);
    jassert(masterTransformPtr          != nullptr);

    jassert(biquadsABypassPtr           != nullptr);
    jassert(biquadsAFrequencyPtr        != nullptr);
    jassert(biquadsAResonancePtr        != nullptr);
    jassert(biquadsAGainPtr             != nullptr);
    jassert(biquadsATypePtr             != nullptr);

    jassert(biquadsBBypassPtr           != nullptr);
    jassert(biquadsBFrequencyPtr        != nullptr);
    jassert(biquadsBResonancePtr        != nullptr);
    jassert(biquadsBGainPtr             != nullptr);
    jassert(biquadsBTypePtr             != nullptr);

    jassert(biquadsCBypassPtr           != nullptr);
    jassert(biquadsCFrequencyPtr        != nullptr);
    jassert(biquadsCResonancePtr        != nullptr);
    jassert(biquadsCGainPtr             != nullptr);
    jassert(biquadsCTypePtr             != nullptr);

    jassert(biquadsDBypassPtr           != nullptr);
    jassert(biquadsDFrequencyPtr        != nullptr);
    jassert(biquadsDResonancePtr        != nullptr);
    jassert(biquadsDGainPtr             != nullptr);
    jassert(biquadsDTypePtr             != nullptr);

    jassert(bypassState                 != nullptr);
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
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    processorFlt.prepare(getSpec());
    processorDbl.prepare(getSpec());
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    processorFlt.reset(0.0f);
    processorDbl.reset(0.0);
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
    jassert (! isUsingDoublePrecision());

    juce::ScopedNoDenormals noDenormals;

    processorFlt.process(buffer, midiMessages);
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    jassert (isUsingDoublePrecision());

    juce::ScopedNoDenormals noDenormals;

    processorDbl.process(buffer, midiMessages);

}

void AudioPluginAudioProcessor::processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    jassert (! isUsingDoublePrecision());

    processorFlt.processBypass(buffer, midiMessages);
}

void AudioPluginAudioProcessor::processBlockBypassed(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    jassert (isUsingDoublePrecision());

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

    AudioPluginAudioProcessorParameters::setParameterLayout(parameterLayout);

    // parameterLayout.add(std::make_unique<juce::AudioParameterBool> (juce::ParameterID{ "bypassID", 1}, "Bypass", false));

    return parameterLayout;
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // auto apvts = parameters.getAPVTS();

    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);

    // // Store an xml representation of our state.
    // if (auto xmlState = state.copyState().createXml())
    //     copyXmlToBinary (*xmlState, destData);
}

void AudioPluginAudioProcessor::getCurrentProgramStateInformation(juce::MemoryBlock& destData)
{
    // using apvts = parameters.getAPVTS();

    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // using apvts = parameters.getAPVTS();
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));

    // // Restore our plug-in's state from the xml representation stored in the above
    // // method.
    // if (auto xmlState = getXmlFromBinary (data, sizeInBytes))
    //     state.replaceState (ValueTree::fromXml (*xmlState));
}

void AudioPluginAudioProcessor::setCurrentProgramStateInformation(const void* data, int sizeInBytes)
{
    // using apvts = parameters.getAPVTS();

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP

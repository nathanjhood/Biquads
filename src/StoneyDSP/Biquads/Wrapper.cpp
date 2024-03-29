/***************************************************************************//**
 * @file Wrapper.cpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief Simple two-pole equalizer with variable oversampling.
 * @version 1.2.2.174
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

template <typename SampleType>
AudioPluginAudioProcessorWrapper<SampleType>::AudioPluginAudioProcessorWrapper(AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts, juce::dsp::ProcessSpec& spec)
: audioProcessor(p)
, state(apvts)
, setup(spec)

, mixer(std::make_unique<juce::dsp::DryWetMixer<SampleType>>())

, biquadArraySize(static_cast<std::size_t>(4)) // cannot ‘dynamic_cast’ this - target  typeis not pointer or reference...

, masterBypassPtr(dynamic_cast <juce::AudioParameterBool*>(apvts.getParameter("Master_bypassID")))
, masterOutputPtr(dynamic_cast <juce::AudioParameterFloat*>(apvts.getParameter("Master_outputID")))
, masterMixPtr(dynamic_cast <juce::AudioParameterFloat*>(apvts.getParameter("Master_mixID")))
, masterOsPtr(dynamic_cast <juce::AudioParameterChoice*>(apvts.getParameter("Master_osID")))
, masterTransformPtr(dynamic_cast <juce::AudioParameterChoice*>(apvts.getParameter("Master_transformID")))

, biquadsABypassPtr(dynamic_cast <juce::AudioParameterBool*>(apvts.getParameter("Band_A_bypassID")))
, biquadsAFrequencyPtr(dynamic_cast <juce::AudioParameterFloat*>(apvts.getParameter("Band_A_frequencyID")))
, biquadsAResonancePtr(dynamic_cast <juce::AudioParameterFloat*>(apvts.getParameter("Band_A_resonanceID")))
, biquadsAGainPtr(dynamic_cast <juce::AudioParameterFloat*>(apvts.getParameter("Band_A_gainID")))
, biquadsATypePtr(dynamic_cast <juce::AudioParameterChoice*>(apvts.getParameter("Band_A_typeID")))

, biquadsBBypassPtr(dynamic_cast <juce::AudioParameterBool*>(apvts.getParameter("Band_B_bypassID")))
, biquadsBFrequencyPtr(dynamic_cast <juce::AudioParameterFloat*>(apvts.getParameter("Band_B_frequencyID")))
, biquadsBResonancePtr(dynamic_cast <juce::AudioParameterFloat*>(apvts.getParameter("Band_B_resonanceID")))
, biquadsBGainPtr(dynamic_cast <juce::AudioParameterFloat*>(apvts.getParameter("Band_B_gainID")))
, biquadsBTypePtr(dynamic_cast <juce::AudioParameterChoice*>(apvts.getParameter("Band_B_typeID")))

, biquadsCBypassPtr(dynamic_cast <juce::AudioParameterBool*>(apvts.getParameter("Band_C_bypassID")))
, biquadsCFrequencyPtr(dynamic_cast <juce::AudioParameterFloat*>(apvts.getParameter("Band_C_frequencyID")))
, biquadsCResonancePtr(dynamic_cast <juce::AudioParameterFloat*>(apvts.getParameter("Band_C_resonanceID")))
, biquadsCGainPtr(dynamic_cast <juce::AudioParameterFloat*>(apvts.getParameter("Band_C_gainID")))
, biquadsCTypePtr(dynamic_cast <juce::AudioParameterChoice*>(apvts.getParameter("Band_C_typeID")))

, biquadsDBypassPtr(dynamic_cast <juce::AudioParameterBool*>(apvts.getParameter("Band_D_bypassID")))
, biquadsDFrequencyPtr(dynamic_cast <juce::AudioParameterFloat*>(apvts.getParameter("Band_D_frequencyID")))
, biquadsDResonancePtr(dynamic_cast <juce::AudioParameterFloat*>(apvts.getParameter("Band_D_resonanceID")))
, biquadsDGainPtr(dynamic_cast <juce::AudioParameterFloat*>(apvts.getParameter("Band_D_gainID")))
, biquadsDTypePtr(dynamic_cast <juce::AudioParameterChoice*>(apvts.getParameter("Band_D_typeID")))

, bypassState(dynamic_cast <juce::AudioParameterBool*>(apvts.getParameter("Master_bypassID")))
{

    masterBypassPtr         = dynamic_cast <juce::AudioParameterBool*>  (apvts.getParameter("Master_bypassID"));
    masterOutputPtr         = dynamic_cast <juce::AudioParameterFloat*> (apvts.getParameter("Master_outputID"));
    masterMixPtr            = dynamic_cast <juce::AudioParameterFloat*> (apvts.getParameter("Master_mixID"));
    masterOsPtr             = dynamic_cast <juce::AudioParameterChoice*>(apvts.getParameter("Master_osID"));
    masterTransformPtr      = dynamic_cast <juce::AudioParameterChoice*>(apvts.getParameter("Master_transformID"));

    biquadsABypassPtr       = dynamic_cast <juce::AudioParameterBool*>  (apvts.getParameter("Band_A_bypassID"));
    biquadsAFrequencyPtr    = dynamic_cast <juce::AudioParameterFloat*> (apvts.getParameter("Band_A_frequencyID"));
    biquadsAResonancePtr    = dynamic_cast <juce::AudioParameterFloat*> (apvts.getParameter("Band_A_resonanceID"));
    biquadsAGainPtr         = dynamic_cast <juce::AudioParameterFloat*> (apvts.getParameter("Band_A_gainID"));
    biquadsATypePtr         = dynamic_cast <juce::AudioParameterChoice*>(apvts.getParameter("Band_A_typeID"));

    biquadsBBypassPtr       = dynamic_cast <juce::AudioParameterBool*>  (apvts.getParameter("Band_B_bypassID"));
    biquadsBFrequencyPtr    = dynamic_cast <juce::AudioParameterFloat*> (apvts.getParameter("Band_B_frequencyID"));
    biquadsBResonancePtr    = dynamic_cast <juce::AudioParameterFloat*> (apvts.getParameter("Band_B_resonanceID"));
    biquadsBGainPtr         = dynamic_cast <juce::AudioParameterFloat*> (apvts.getParameter("Band_B_gainID"));
    biquadsBTypePtr         = dynamic_cast <juce::AudioParameterChoice*>(apvts.getParameter("Band_B_typeID"));

    biquadsCBypassPtr       = dynamic_cast <juce::AudioParameterBool*>  (apvts.getParameter("Band_C_bypassID"));
    biquadsCFrequencyPtr    = dynamic_cast <juce::AudioParameterFloat*> (apvts.getParameter("Band_C_frequencyID"));
    biquadsCResonancePtr    = dynamic_cast <juce::AudioParameterFloat*> (apvts.getParameter("Band_C_resonanceID"));
    biquadsCGainPtr         = dynamic_cast <juce::AudioParameterFloat*> (apvts.getParameter("Band_C_gainID"));
    biquadsCTypePtr         = dynamic_cast <juce::AudioParameterChoice*>(apvts.getParameter("Band_C_typeID"));

    biquadsDBypassPtr       = dynamic_cast <juce::AudioParameterBool*>  (apvts.getParameter("Band_D_bypassID"));
    biquadsDFrequencyPtr    = dynamic_cast <juce::AudioParameterFloat*> (apvts.getParameter("Band_D_frequencyID"));
    biquadsDResonancePtr    = dynamic_cast <juce::AudioParameterFloat*> (apvts.getParameter("Band_D_resonanceID"));
    biquadsDGainPtr         = dynamic_cast <juce::AudioParameterFloat*> (apvts.getParameter("Band_D_gainID"));
    biquadsDTypePtr         = dynamic_cast <juce::AudioParameterChoice*>(apvts.getParameter("Band_D_typeID"));

    bypassState             = dynamic_cast <juce::AudioParameterBool*>  (apvts.getParameter("Master_bypassID"));

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

    jassert(biquadArraySize             != static_cast<std::size_t>(0));

    // auto osFilter = juce::dsp::Oversampling<SampleType>::filterHalfBandFIREquiripple;

    // for (int i = 0; i < 5; ++i)
    //     oversampler[i] = std::make_unique<juce::dsp::Oversampling<SampleType>>
    //     (audioProcessor.getTotalNumInputChannels(), i, osFilter, true, false);

    for (std::size_t i = 0; i < biquadArraySize; ++i)
        biquadArray[i] = std::make_unique<StoneyDSP::Audio::Biquads<SampleType>>();

    reset(static_cast<SampleType>(0.0));
}

template<class SampleType> AudioPluginAudioProcessorWrapper<SampleType>::~AudioPluginAudioProcessorWrapper()
{
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    // jassert((biquadArray.size() == biquadArraySize) && (biquadArraySize != static_cast<std::size_t>(0)));

    sampleRate = spec.sampleRate;

    // oversamplingFactor = 1 << curOS;
    // prevOS = curOS;

    // for (int i = 0; i < 5; ++i)
    //     oversampler[i]->initProcessing(spec.maximumBlockSize);

    // for (int i = 0; i < 5; ++i)
    //     oversampler[i]->numChannels = (size_t)spec.numChannels;

    reset(static_cast<SampleType>(0.0));

    mixer->prepare(spec);

    for(auto& biquad : biquadArray)
        biquad->prepare(spec);

    update();
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::reset()
{
    SampleType initialValue = static_cast<SampleType>(0.0);

    mixer->reset();

    for(auto& biquad : biquadArray)
        biquad->reset(initialValue);

    // for (int i = 0; i < 5; ++i)
    //     oversampler[i]->reset();
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::reset(SampleType initialValue)
{
    mixer->reset();

    for(auto& biquad : biquadArray)
        biquad->reset(initialValue);

    // for (int i = 0; i < 5; ++i)
    //     oversampler[i]->reset();
}

//==============================================================================
template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    auto totalNumInputChannels  = audioProcessor.getTotalNumInputChannels();
    auto totalNumOutputChannels = audioProcessor.getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);

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
        // ..do something to the data... (mixer push wet samples)?
    }

    // setOversampling();
    update();

    processBlock(buffer, midiMessages);
    return;
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::processBlock(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::dsp::AudioBlock<SampleType> dryBlock(buffer);
    juce::dsp::AudioBlock<SampleType> wetBlock(buffer);

    mixer->pushDrySamples(dryBlock);

    // wetBlock = oversampler[curOS]->processSamplesUp(dryBlock);

    // This context is intended for use in situations where two different blocks
    // are being used as the input and output to the process algorithm, so the
    // processor must read from the block returned by getInputBlock() and write
    // its results to the block returned by getOutputBlock().
    auto context = juce::dsp::ProcessContextReplacing<SampleType> (wetBlock);

    for(auto& biquad : biquadArray)
        biquad->process(context);

    // processContext(context);

    // oversampler[curOS]->processSamplesDown(wetBlock);

    // applyGain(buffer, static_cast<SampleType>(juce::Decibels::decibelsToGain(static_cast<SampleType>(masterOutputPtr->get()), static_cast<SampleType>(-120.00))))

    mixer->mixWetSamples(wetBlock);
    return;
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::processBypass(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::dsp::AudioBlock<SampleType> block(buffer);

    // This context is intended for use in situations where a single block is
    // being used for both the input and output to the process algorithm, so it
    // will return the same object for both its getInputBlock() and
    // getOutputBlock() methods.
    auto context = juce::dsp::ProcessContextReplacing<SampleType>(block);

    const auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();
    const auto numChannels = outputBlock.getNumChannels();
    const auto numSamples = outputBlock.getNumSamples();

    jassert(inputBlock.getNumChannels() == numChannels);
    jassert(inputBlock.getNumSamples() == numSamples);

    juce::ignoreUnused(numChannels);
    juce::ignoreUnused(numSamples);

    outputBlock.copyFrom(inputBlock);
    return;
}

template <typename SampleType>
SampleType AudioPluginAudioProcessorWrapper<SampleType>::processSample(int channel, SampleType inputValue)
{
    juce::ignoreUnused(channel);

    auto& sample = inputValue;
    return sample;

}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::snapToZero() noexcept
{
    // mixer->snapToZero(); // ?

    for(auto& biquad : biquadArray)
        biquad->snapToZero();
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::update()
{
    mixer->setWetMixProportion(static_cast   <SampleType>    (0.01f * masterMixPtr->get()));

    for(auto& biquad : biquadArray)
        biquad->setTransformType(static_cast   <StoneyDSP::Audio::BiquadsBiLinearTransformationType>  (masterTransformPtr->getIndex()));

    biquadArray[0]->setFrequency     (static_cast   <SampleType>                                           (biquadsAFrequencyPtr->get()));
    biquadArray[0]->setResonance     (static_cast   <SampleType>                                           (biquadsAResonancePtr->get()));
    biquadArray[0]->setGain          (static_cast   <SampleType>                                           (biquadsAGainPtr->get()));
    biquadArray[0]->setFilterType    (static_cast   <StoneyDSP::Audio::BiquadsFilterType>                  (biquadsATypePtr->getIndex()));

    biquadArray[1]->setFrequency     (static_cast   <SampleType>                                           (biquadsBFrequencyPtr->get()));
    biquadArray[1]->setResonance     (static_cast   <SampleType>                                           (biquadsBResonancePtr->get()));
    biquadArray[1]->setGain          (static_cast   <SampleType>                                           (biquadsBGainPtr->get()));
    biquadArray[1]->setFilterType    (static_cast   <StoneyDSP::Audio::BiquadsFilterType>                  (biquadsBTypePtr->getIndex()));

    biquadArray[2]->setFrequency     (static_cast   <SampleType>                                           (biquadsCFrequencyPtr->get()));
    biquadArray[2]->setResonance     (static_cast   <SampleType>                                           (biquadsCResonancePtr->get()));
    biquadArray[2]->setGain          (static_cast   <SampleType>                                           (biquadsCGainPtr->get()));
    biquadArray[2]->setFilterType    (static_cast   <StoneyDSP::Audio::BiquadsFilterType>                  (biquadsCTypePtr->getIndex()));

    biquadArray[3]->setFrequency     (static_cast   <SampleType>                                           (biquadsDFrequencyPtr->get()));
    biquadArray[3]->setResonance     (static_cast   <SampleType>                                           (biquadsDResonancePtr->get()));
    biquadArray[3]->setGain          (static_cast   <SampleType>                                           (biquadsDGainPtr->get()));
    biquadArray[3]->setFilterType    (static_cast   <StoneyDSP::Audio::BiquadsFilterType>                  (biquadsDTypePtr->getIndex()));
}

// template <typename SampleType>
// void AudioPluginAudioProcessorWrapper<SampleType>::setOversampling()
// {
//     curOS = (int)osPtr->getIndex();
//     if (curOS != prevOS)
//     {
//         oversamplingFactor = 1 << curOS;
//         prevOS = curOS;
//         mixer.reset();
//         // biquad.reset();
//     }
// }

// template <typename SampleType>
// SampleType AudioPluginAudioProcessorWrapper<SampleType>::getLatencySamples() const noexcept
// {
//     return oversampler[curOS]->getLatencyInSamples();
// }
//==============================================================================
template class AudioPluginAudioProcessorWrapper<float>;
template class AudioPluginAudioProcessorWrapper<double>;

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP

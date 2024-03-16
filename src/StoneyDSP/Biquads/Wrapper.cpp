/***************************************************************************//**
 * @file Wrapper.cpp
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

template <typename SampleType>
AudioPluginAudioProcessorWrapper<SampleType>::AudioPluginAudioProcessorWrapper(AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts, juce::dsp::ProcessSpec& spec)
: audioProcessor (p)
, state(apvts)
, setup(spec)
, mixer()
, biquads()
, frequencyPtr  (dynamic_cast <juce::AudioParameterFloat*>  (p.getAPVTS().getParameter("Band_A_frequencyID")))
, resonancePtr  (dynamic_cast <juce::AudioParameterFloat*>  (p.getAPVTS().getParameter("Band_A_resonanceID")))
, gainPtr       (dynamic_cast <juce::AudioParameterFloat*>  (p.getAPVTS().getParameter("Band_A_gainID")))
, typePtr       (dynamic_cast <juce::AudioParameterChoice*> (p.getAPVTS().getParameter("Band_A_typeID")))
, transformPtr  (dynamic_cast <juce::AudioParameterChoice*> (p.getAPVTS().getParameter("Master_transformID")))
, osPtr         (dynamic_cast <juce::AudioParameterChoice*> (p.getAPVTS().getParameter("Master_osID")))
, outputPtr     (dynamic_cast <juce::AudioParameterFloat*>  (p.getAPVTS().getParameter("Master_outputID")))
, mixPtr        (dynamic_cast <juce::AudioParameterFloat*>  (p.getAPVTS().getParameter("Master_mixID")))
{

    jassert(frequencyPtr    != nullptr);
    jassert(resonancePtr    != nullptr);
    jassert(gainPtr         != nullptr);
    jassert(typePtr         != nullptr);
    jassert(transformPtr    != nullptr);
    jassert(osPtr           != nullptr);
    jassert(outputPtr       != nullptr);
    jassert(mixPtr          != nullptr);

    // auto osFilter = juce::dsp::Oversampling<SampleType>::filterHalfBandFIREquiripple;

    // for (int i = 0; i < 5; ++i)
    //     oversampler[i] = std::make_unique<juce::dsp::Oversampling<SampleType>>
    //     (audioProcessor.getTotalNumInputChannels(), i, osFilter, true, false);

    reset(static_cast<SampleType>(0.0));
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    sampleRate = spec.sampleRate;

    // oversamplingFactor = 1 << curOS;
    // prevOS = curOS;

    // for (int i = 0; i < 5; ++i)
    //     oversampler[i]->initProcessing(spec.maximumBlockSize);

    // for (int i = 0; i < 5; ++i)
    //     oversampler[i]->numChannels = (size_t)spec.numChannels;

    reset(static_cast<SampleType>(0.0));

    mixer.prepare(spec);
    biquads.prepare(spec);

    update();
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::reset()
{
    mixer.reset();
    biquads.reset(static_cast<SampleType>(0.0));

    // for (int i = 0; i < 5; ++i)
    //     oversampler[i]->reset();
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::reset(SampleType initialValue)
{
    mixer.reset();
    biquads.reset(initialValue);

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

    mixer.pushDrySamples(dryBlock);

    // wetBlock = oversampler[curOS]->processSamplesUp(dryBlock);

    // This context is intended for use in situations where two different blocks
    // are being used as the input and output to the process algorithm, so the
    // processor must read from the block returned by getInputBlock() and write
    // its results to the block returned by getOutputBlock().
    auto context = juce::dsp::ProcessContextReplacing<SampleType> (wetBlock);

    // biquad.process(context);
    processContext(context);

    // oversampler[curOS]->processSamplesDown(wetBlock);

    for (auto channel = 0; channel < audioProcessor.getTotalNumOutputChannels(); ++channel)
        buffer.applyGain (channel, 0, buffer.getNumSamples(), static_cast<SampleType>(juce::Decibels::decibelsToGain(static_cast<SampleType>(outputPtr->get()), static_cast<SampleType>(-120.00))));

    mixer.mixWetSamples(wetBlock);
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
    return biquads.processSample(channel, inputValue);
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::snapToZero() noexcept
{
    biquads.snapToZero();
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::update()
{
    mixer.setWetMixProportion(static_cast   <SampleType>    (mixPtr->get() * 0.01));
    biquads.setFrequency     (static_cast   <SampleType>    (frequencyPtr->get()));
    biquads.setResonance     (static_cast   <SampleType>    (resonancePtr->get()));
    biquads.setGain          (static_cast   <SampleType>    (gainPtr->get()));
    biquads.setFilterType    (static_cast   <StoneyDSP::Audio::BiquadsFilterType>                   (typePtr->getIndex()));
    biquads.setTransformType (static_cast   <StoneyDSP::Audio::BiquadsBiLinearTransformationType>   (transformPtr->getIndex()));
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

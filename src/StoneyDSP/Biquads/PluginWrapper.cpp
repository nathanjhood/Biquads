/**
 * @file PluginWrapper.cpp
 * @author StoneyDSP (nathanjhood@googlemail.com)
 * @brief
 * @version 0.1
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "StoneyDSP/Biquads/PluginWrapper.hpp"
#include "StoneyDSP/Biquads/PluginProcessor.hpp"

namespace StoneyDSP {
namespace Biquads {

template <typename SampleType>
AudioPluginAudioProcessWrapper<SampleType>::AudioPluginAudioProcessWrapper(AudioPluginAudioProcessor& p)
: audioProcessor (p)
, state (p.getAPVTS())
, setup (p.getSpec())
, mixer()
{
    reset();
}

template <typename SampleType>
void AudioPluginAudioProcessWrapper<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    mixer.prepare(spec);
    reset();
    update();
}

template <typename SampleType>
void AudioPluginAudioProcessWrapper<SampleType>::reset()
{
    mixer.reset();
}

// //==============================================================================
template <typename SampleType>
void AudioPluginAudioProcessWrapper<SampleType>::process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::dsp::AudioBlock<SampleType> block(buffer);

    // This context is intended for use in situations where two different blocks
    // are being used as the input and output to the process algorithm, so the
    // processor must read from the block returned by getInputBlock() and write
    // its results to the block returned by getOutputBlock().
    juce::dsp::ProcessContextReplacing<SampleType> context(block);

    const auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();

    outputBlock.copyFrom(inputBlock);
}

template <typename SampleType>
void AudioPluginAudioProcessWrapper<SampleType>::processBypass(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::dsp::AudioBlock<SampleType> block(buffer);

    // This context is intended for use in situations where a single block is
    // being used for both the input and output to the process algorithm, so it
    // will return the same object for both its getInputBlock() and
    // getOutputBlock() methods.
    juce::dsp::ProcessContextReplacing<SampleType> context(block);

    const auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();

    outputBlock.copyFrom(inputBlock);
}

template <typename SampleType>
void AudioPluginAudioProcessWrapper<SampleType>::update()
{
}
//==============================================================================
template class AudioPluginAudioProcessWrapper<float>;
template class AudioPluginAudioProcessWrapper<double>;

} // namespace StoneyDSP
} // namespace Biquads

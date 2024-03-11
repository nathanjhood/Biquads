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
AudioPluginAudioProcessorWrapper<SampleType>::AudioPluginAudioProcessorWrapper(AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts, juce::dsp::ProcessSpec& spec)
: audioProcessor (p)
, state (apvts)
, setup (spec)
// , coeff((<SampleType>)0.5)
, mixer()
, outputPtr   (dynamic_cast<juce::AudioParameterFloat*>(state.getParameter("outputID")))
{
    jassert(outputPtr != nullptr);

    reset();
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    setup.sampleRate        = spec.sampleRate;
    setup.maximumBlockSize  = spec.maximumBlockSize;
    setup.numChannels       = spec.numChannels;

    reset();

    mixer.prepare(setup);

    update();
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::reset()
{
    mixer.reset();
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

    processBlock(buffer, midiMessages);
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::processBlock(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    auto gainParamValue  = state.getParameter ("outputID")->getValue();

    juce::dsp::AudioBlock<SampleType> dryBlock(buffer);
    juce::dsp::AudioBlock<SampleType> wetBlock(buffer);

    // This context is intended for use in situations where two different blocks
    // are being used as the input and output to the process algorithm, so the
    // processor must read from the block returned by getInputBlock() and write
    // its results to the block returned by getOutputBlock().
    auto context = juce::dsp::ProcessContextReplacing<SampleType> (wetBlock);

    const auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();

    outputBlock.copyFrom(inputBlock);

    for (auto channel = 0; channel < audioProcessor.getTotalNumOutputChannels(); ++channel)
        buffer.applyGain (channel, 0, buffer.getNumSamples(), gainParamValue);

    mixer.mixWetSamples(outputBlock);
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

    outputBlock.copyFrom(inputBlock);

    mixer.pushDrySamples(outputBlock);
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::update()
{
}
//==============================================================================
template class AudioPluginAudioProcessorWrapper<float>;
template class AudioPluginAudioProcessorWrapper<double>;

} // namespace StoneyDSP
} // namespace Biquads

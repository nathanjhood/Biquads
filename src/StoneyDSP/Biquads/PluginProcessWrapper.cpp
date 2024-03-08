#include "StoneyDSP/Biquads/PluginProcessWrapper.hpp"

template <typename SampleType>
ProcessWrapper<SampleType>::ProcessWrapper(BiquadsAudioProcessor& p)
: audioProcessor (p)
, state (p.getAPVTS())
, setup (p.getSpec())
, mixer ()
, output ()
{
    reset();
}
template <typename SampleType>
void ProcessWrapper<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    reset();
    update();
}

template <typename SampleType>
void ProcessWrapper<SampleType>::update()
{
}

template <typename SampleType>
void ProcessWrapper<SampleType>::reset()
{
}

//==============================================================================
template <typename SampleType>
void ProcessWrapper<SampleType>::process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    // juce::ignoreUnused (midiMessages);
    midiMessages.clear();

    juce::dsp::AudioBlock<SampleType> block(buffer);

    mixer.pushDrySamples(block);

    juce::dsp::ProcessContextReplacing context(block);

    output.process(context);

    mixer.mixWetSamples(block);
}
//==============================================================================
template class ProcessWrapper<float>;
template class ProcessWrapper<double>;

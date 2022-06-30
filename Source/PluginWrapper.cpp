/*
  ==============================================================================

    PluginWrapper.cpp
    Created: 8 May 2022 9:38:17pm
    Author:  StoneyDSP

  ==============================================================================
*/

#include "PluginWrapper.h"
#include "PluginProcessor.h"

template <typename SampleType>
ProcessWrapper<SampleType>::ProcessWrapper(BiquadsAudioProcessor& p)
    : 
    audioProcessor (p), 
    state (p.getAPVTS()),
    setup (p.getSpec()),
    mixer (),
    biquad (),
    output (), 
    frequencyPtr (dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("frequencyID"))),
    resonancePtr (dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("resonanceID"))),
    gainPtr (dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("gainID"))),
    typePtr(dynamic_cast <juce::AudioParameterChoice*> (p.getAPVTS().getParameter("typeID"))),
    transformPtr (dynamic_cast <juce::AudioParameterChoice*> (p.getAPVTS().getParameter("transformID"))),
    osPtr (dynamic_cast <juce::AudioParameterChoice*> (p.getAPVTS().getParameter("osID"))),
    outputPtr (dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("outputID"))),
    mixPtr (dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("mixID"))),
    bypassPtr(dynamic_cast <juce::AudioParameterBool*> (p.getAPVTS().getParameter("bypassID")))
{
    jassert(frequencyPtr != nullptr);
    jassert(resonancePtr != nullptr);
    jassert(gainPtr != nullptr);
    jassert(typePtr != nullptr);
    jassert(transformPtr != nullptr);
    jassert(osPtr != nullptr);
    jassert(outputPtr != nullptr);
    jassert(mixPtr != nullptr);
    jassert(bypassPtr != nullptr);

    auto osFilter = juce::dsp::Oversampling<SampleType>::filterHalfBandFIREquiripple;

    for (int i = 0; i < 5; ++i)
        oversampler[i] = std::make_unique<juce::dsp::Oversampling<SampleType>>
        (audioProcessor.getTotalNumInputChannels(), i, osFilter, true, false);

    reset();
}


template <typename SampleType>
void ProcessWrapper<SampleType>::prepare(Spec& spec)
{
    oversamplingFactor = 1 << curOS;
    prevOS = curOS;

    for (int i = 0; i < 5; ++i)
        oversampler[i]->initProcessing(spec.maximumBlockSize);

    for (int i = 0; i < 5; ++i)
        oversampler[i]->numChannels = (size_t)spec.numChannels;

    mixer.prepare(spec);
    biquad.prepare(spec);
    output.prepare(spec);

    reset();
    update();
}

template <typename SampleType>
void ProcessWrapper<SampleType>::reset()
{
    mixer.reset();
    biquad.reset();
    output.reset();

    for (int i = 0; i < 5; ++i)
        oversampler[i]->reset();
}

//==============================================================================
template <typename SampleType>
void ProcessWrapper<SampleType>::process(Audio& buffer, Midi& midiMessages)
{
    midiMessages.clear();

    update();

    juce::dsp::AudioBlock<SampleType> block(buffer);
    juce::dsp::AudioBlock<SampleType> osBlock(buffer);

    mixer.pushDrySamples(block);

    osBlock = oversampler[curOS]->processSamplesUp(block);

    auto context = juce::dsp::ProcessContextReplacing(osBlock);

    biquad.process(context);

    output.process(context);

    oversampler[curOS]->processSamplesDown(block);

    mixer.mixWetSamples(block);
}

template <typename SampleType>
void ProcessWrapper<SampleType>::bypass(Audio& buffer, Midi& midiMessages)
{
    juce::ignoreUnused(buffer);
    midiMessages.clear();
}

template <typename SampleType>
void ProcessWrapper<SampleType>::update()
{
    setOversampling();

    mixer.setWetMixProportion(mixPtr->get() * 0.01f);

    biquad.setFrequency(frequencyPtr->get());
    biquad.setResonance(resonancePtr->get());
    biquad.setGain(gainPtr->get());

    biquad.setFilterType(static_cast<FilterType>(typePtr->getIndex()));
    biquad.setTransformType(static_cast<TransformationType>(transformPtr->getIndex()));

    output.setGainLinear(juce::Decibels::decibelsToGain(outputPtr->get()));
}

template <typename SampleType>
void ProcessWrapper<SampleType>::setOversampling()
{
    curOS = (int)osPtr->getIndex();
    if (curOS != prevOS)
    {
        oversamplingFactor = 1 << curOS;
        prevOS = curOS;
        mixer.reset();
        //mixer.setWetLatency(getLatencySamples());
        biquad.reset();
        output.reset();
    }
}

template <typename SampleType>
SampleType ProcessWrapper<SampleType>::getLatencySamples() const noexcept
{
    return oversampler[curOS]->getLatencyInSamples();
}
//==============================================================================
template class ProcessWrapper<float>;
template class ProcessWrapper<double>;
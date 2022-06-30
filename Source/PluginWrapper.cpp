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
    state(p.getAPVTS ()),
    setup (p.getSpec()),
    mixer (),
    biquad (),
    output (), 
    /*frq (), res (), lev (),*/
    frequencyPtr(dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("frequencyID"))),
    resonancePtr(dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("resonanceID"))),
    gainPtr(dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("gainID"))),
    typePtr(dynamic_cast <juce::AudioParameterChoice*> (p.getAPVTS().getParameter("typeID"))),
    transformPtr(dynamic_cast <juce::AudioParameterChoice*> (p.getAPVTS().getParameter("transformID"))),
    osPtr(dynamic_cast <juce::AudioParameterChoice*> (p.getAPVTS().getParameter("osID"))),
    outputPtr(dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("outputID"))),
    mixPtr(dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("mixID"))),
    bypassPtr(dynamic_cast <juce::AudioParameterBool*> (p.getAPVTS().getParameter("bypassID")))
{
    setup.sampleRate = audioProcessor.getSampleRate();
    setup.maximumBlockSize = audioProcessor.getBlockSize();
    setup.numChannels = audioProcessor.getTotalNumInputChannels();

    auto osFilter = juce::dsp::Oversampling<SampleType>::filterHalfBandPolyphaseIIR;

    for (int i = 0; i < 5; ++i)
        oversampler[i] = std::make_unique<juce::dsp::Oversampling<SampleType>>
        (audioProcessor.getTotalNumInputChannels(), i, osFilter, true, false);

    jassert(frequencyPtr != nullptr);
    jassert(resonancePtr != nullptr);
    jassert(gainPtr != nullptr);
    jassert(typePtr != nullptr);
    jassert(transformPtr != nullptr);
    jassert(osPtr != nullptr);
    jassert(outputPtr != nullptr);
    jassert(mixPtr != nullptr);
    jassert(bypassPtr != nullptr);

    /*frq.setTargetValue(frequencyPtr->get());
    res.setTargetValue(resonancePtr->get());
    lev.setTargetValue(gainPtr->get());*/
}

//template <typename SampleType>
//void ProcessWrapper<SampleType>::call(Caller& calls)
//{
//    calls.timerCallback();
//}

//==============================================================================
//template <typename SampleType>
//void ProcessWrapper<SampleType>::setRampDurationSeconds(double newDurationSeconds) noexcept
//{
//    if (rampDurationSeconds != newDurationSeconds)
//    {
//        rampDurationSeconds = newDurationSeconds;
//        reset();
//    }
//}
//
//template <typename SampleType>
//double ProcessWrapper<SampleType>::getRampDurationSeconds() const noexcept
//{
//    return rampDurationSeconds;
//}
//
//template <typename SampleType>
//bool ProcessWrapper<SampleType>::isSmoothing() const noexcept
//{
//    bool smoothingActive = frq.isSmoothing() || res.isSmoothing() || lev.isSmoothing();
//
//    return smoothingActive;
//}


template <typename SampleType>
void ProcessWrapper<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    oversamplingFactor = 1 << curOS;
    prevOS = curOS;

    spec.sampleRate = audioProcessor.getSampleRate();
    spec.maximumBlockSize = audioProcessor.getBlockSize();
    spec.numChannels = audioProcessor.getTotalNumInputChannels();

    for (int i = 0; i < 5; ++i)
        oversampler[i]->initProcessing(spec.maximumBlockSize);

    for (int i = 0; i < 5; ++i)
        oversampler[i]->numChannels = (size_t)spec.numChannels;

    mixer.prepare(spec);
    biquad.prepare(spec);
    output.prepare(spec);

    reset();

    audioProcessor.setBypassParameter(bypassPtr);

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
void ProcessWrapper<SampleType>::reset()
{
    mixer.reset();
    mixer.setWetLatency(getLatencySamples());
    biquad.reset();
    output.reset();

    for (int i = 0; i < 5; ++i)
        oversampler[i]->reset();

    /*frq.reset(setup.sampleRate, rampDurationSeconds);
    res.reset(setup.sampleRate, rampDurationSeconds);
    lev.reset(setup.sampleRate, rampDurationSeconds);*/
}

//==============================================================================
template <typename SampleType>
void ProcessWrapper<SampleType>::process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    midiMessages.clear();

    update();

    juce::dsp::AudioBlock<SampleType> block(buffer);
    juce::dsp::AudioBlock<SampleType> osBlock(buffer);

    mixer.pushDrySamples(block);

    osBlock = oversampler[curOS]->processSamplesUp(block);

    auto context = juce::dsp::ProcessContextReplacing(osBlock);

    context.isBypassed = bypassPtr->get();

    /*const auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();
    const auto numChannels = outputBlock.getNumChannels();
    const auto numSamples = outputBlock.getNumSamples();

    if (context.isBypassed)
    {

        outputBlock.copyFrom(inputBlock);
        return;
    }

    for (size_t channel = 0; channel < numChannels; ++channel)
    {
        auto* inputSamples = inputBlock.getChannelPointer(channel);
        auto* outputSamples = outputBlock.getChannelPointer(channel);

        for (size_t i = 0; i < numSamples; ++i)
            outputSamples[i] = biquad.processSample((int)channel, inputSamples[i]);
    }*/

    biquad.process(context);

    output.process(context);

    oversampler[curOS]->processSamplesDown(block);

    mixer.mixWetSamples(block);
}

template <typename SampleType>
void ProcessWrapper<SampleType>::update()
{
    setup.sampleRate = audioProcessor.getSampleRate();
    setup.maximumBlockSize = audioProcessor.getBlockSize();
    setup.numChannels = audioProcessor.getTotalNumInputChannels();

    audioProcessor.setBypassParameter(bypassPtr);

    setOversampling();

    mixer.setWetMixProportion(mixPtr->get() * 0.01f);
    
    /*frq.setTargetValue(frequencyPtr->get());
    res.setTargetValue(resonancePtr->get());
    lev.setTargetValue(gainPtr->get());*/

    biquad.setFrequency(frequencyPtr->get());
    biquad.setResonance(resonancePtr->get());
    biquad.setGain(gainPtr->get());

    /*while (isSmoothing())
    {
        biquad.setFrequency(frequencyPtr->get());
        biquad.setResonance(resonancePtr->get());
        biquad.setGain(gainPtr->get());
    }*/

    biquad.setFilterType(static_cast<FilterType>(typePtr->getIndex()));
    biquad.setTransformType(static_cast<TransformationType>(transformPtr->getIndex()));

    output.setGainLinear(juce::Decibels::decibelsToGain(outputPtr->get()));
}

//template <typename SampleType>
//APVTS ProcessWrapper<SampleType>::timerCallback()
//{
//    while (state.timerCallback())
//    {
//        audioProcessor.setBypassParameter(bypassPtr);
//
//        setOversampling();
//
//        mixer.setWetMixProportion(mixPtr->get() * 0.01f);
//
//        biquad.setFrequency(frequencyPtr->get());
//        biquad.setResonance(resonancePtr->get());
//        biquad.setGain(gainPtr->get());
//        biquad.setFilterType(static_cast<stoneydsp::filters::FilterType>(typePtr->getIndex()));
//        biquad.setTransformType(static_cast<stoneydsp::filters::TransformationType>(transformPtr->getIndex()));
//
//        output.setGainLinear(juce::Decibels::decibelsToGain(outputPtr->get()));
//    }
//
//    return;
//}

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
    // latency of oversampling
    return oversampler[curOS]->getLatencyInSamples();
}
//==============================================================================
template class ProcessWrapper<float>;
template class ProcessWrapper<double>;
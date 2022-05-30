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
ProcessWrapper<SampleType>::ProcessWrapper(BiquadsAudioProcessor& p, AudioProcessorValueTreeState& apvts) : audioProcessor(p)
{
    ioPtr = dynamic_cast                <juce::AudioParameterBool*>         (apvts.getParameter("ioID"));
    jassert(ioPtr != nullptr);

    frequencyPtr = dynamic_cast         <juce::AudioParameterFloat*>        (apvts.getParameter("frequencyID"));
    jassert(frequencyPtr != nullptr);

    resonancePtr = dynamic_cast         <juce::AudioParameterFloat*>        (apvts.getParameter("resonanceID"));
    jassert(resonancePtr != nullptr);

    gainPtr = dynamic_cast              <juce::AudioParameterFloat*>        (apvts.getParameter("gainID"));
    jassert(gainPtr != nullptr);

    typePtr = dynamic_cast              <juce::AudioParameterChoice*>       (apvts.getParameter("typeID"));
    jassert(typePtr != nullptr);

    transformPtr = dynamic_cast         <juce::AudioParameterChoice*>       (apvts.getParameter("transformID"));
    jassert(transformPtr != nullptr);

    osPtr = dynamic_cast                <juce::AudioParameterChoice*>       (apvts.getParameter("osID"));
    jassert(osPtr != nullptr);

    outputPtr = dynamic_cast            <juce::AudioParameterFloat*>        (apvts.getParameter("outputID"));
    jassert(outputPtr != nullptr);

    mixPtr = dynamic_cast              <juce::AudioParameterFloat*>        (apvts.getParameter("mixID"));
    jassert(mixPtr != nullptr);

    auto osChannels = audioProcessor.getTotalNumInputChannels();

    auto osFilter = juce::dsp::Oversampling<SampleType>::filterHalfBandPolyphaseIIR;

    for (int i = 0; i < 5; ++i)
        overSample[i] = std::make_unique<juce::dsp::Oversampling<SampleType>>
        (osChannels, i, osFilter, true, false);
}

template <typename SampleType>
void ProcessWrapper<SampleType>::setOversampling()
{
    curOS = (int)osPtr->getIndex();
    if (curOS != prevOS)
    {
        overSamplingFactor = 1 << curOS;
        prevOS = curOS;
        mixer.reset();
        biquad.reset(static_cast<SampleType>(0.0));
        biquad.sampleRate = spec.sampleRate * overSamplingFactor;
        output.reset();
    }
}

template <typename SampleType>
void ProcessWrapper<SampleType>::prepare(double sampleRate, int samplesPerBlock)
{
    overSamplingFactor = 1 << curOS;
    prevOS = curOS;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = audioProcessor.getTotalNumInputChannels();

    for (int i = 0; i < 5; ++i)
        overSample[i]->initProcessing(spec.maximumBlockSize);

    for (int i = 0; i < 5; ++i)
        overSample[i]->numChannels = (size_t)spec.numChannels;

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
    biquad.reset(static_cast<SampleType>(0.0));
    output.reset();

    for (int i = 0; i < 5; ++i)
        overSample[i]->numChannels = (size_t)spec.numChannels;

    for (int i = 0; i < 5; ++i)
        overSample[i]->reset();
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

//==============================================================================
template <typename SampleType>
void ProcessWrapper<SampleType>::process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    update();

    juce::dsp::AudioBlock<SampleType> block(buffer);

    mixer.pushDrySamples(block);

    juce::dsp::AudioBlock<SampleType> osBlock = overSample[curOS]->processSamplesUp(block);

    auto context = juce::dsp::ProcessContextReplacing(osBlock);

    if (ioPtr->get() == true)
        context.isBypassed = true;
    else
        context.isBypassed = false;

    biquad.process(context);

    output.process(context);

    overSample[curOS]->processSamplesDown(block);

    mixer.mixWetSamples(block);
}

//==============================================================================
template class ProcessWrapper<float>;
template class ProcessWrapper<double>;
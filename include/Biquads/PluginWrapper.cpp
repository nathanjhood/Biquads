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
    mixPtr (dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("mixID")))
{
    jassert(frequencyPtr != nullptr);
    jassert(resonancePtr != nullptr);
    jassert(gainPtr != nullptr);
    jassert(typePtr != nullptr);
    jassert(transformPtr != nullptr);
    jassert(osPtr != nullptr);
    jassert(outputPtr != nullptr);
    jassert(mixPtr != nullptr);

    auto osFilter = juce::dsp::Oversampling<SampleType>::filterHalfBandFIREquiripple;

    for (int i = 0; i < 5; ++i)
        oversampler[i] = std::make_unique<juce::dsp::Oversampling<SampleType>>
        (audioProcessor.getTotalNumInputChannels(), i, osFilter, true, false);

    reset();
}

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
void ProcessWrapper<SampleType>::process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    midiMessages.clear();

    setOversampling();
    update();

    juce::dsp::AudioBlock<SampleType> block(buffer);
    juce::dsp::AudioBlock<SampleType> osBlock(buffer);

    mixer.pushDrySamples(block);

    osBlock = oversampler[curOS]->processSamplesUp(block);

    juce::dsp::ProcessContextReplacing context(osBlock);

    biquad.process(context);

    output.process(context);

    oversampler[curOS]->processSamplesDown(block);

    mixer.mixWetSamples(block);
}

template <typename SampleType>
void ProcessWrapper<SampleType>::update()
{
    mixer.setWetMixProportion(mixPtr->get() * 0.01f);
    biquad.setFrequency(frequencyPtr->get() / oversamplingFactor);
    biquad.setResonance(resonancePtr->get());
    biquad.setGain(gainPtr->get());
    biquad.setFilterType(static_cast<FilterType>(typePtr->getIndex()));
    biquad.setTransformType(static_cast<TransformationType>(transformPtr->getIndex()));
    output.setGainDecibels(outputPtr->get());
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

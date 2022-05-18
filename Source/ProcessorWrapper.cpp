/*
  ==============================================================================

    ProcessorWrapper.cpp
    Created: 8 May 2022 9:38:17pm
    Author:  StoneyDSP

  ==============================================================================
*/

#include "ProcessorWrapper.h"

template <typename SampleType>
ProcessWrapper<SampleType>::ProcessWrapper(AudioProcessorValueTreeState& apvts)
{
    ioPtr = dynamic_cast       <juce::AudioParameterBool*>    (apvts.getParameter("ioID"));
    jassert(ioPtr != nullptr);

    frequencyPtr = dynamic_cast      <juce::AudioParameterFloat*>    (apvts.getParameter("frequencyID"));
    jassert(frequencyPtr != nullptr);

    resonancePtr = dynamic_cast      <juce::AudioParameterFloat*>    (apvts.getParameter("resonanceID"));
    jassert(resonancePtr != nullptr);

    gainPtr = dynamic_cast      <juce::AudioParameterFloat*>    (apvts.getParameter("gainID"));
    jassert(gainPtr != nullptr);

    typePtr = dynamic_cast      <juce::AudioParameterChoice*>    (apvts.getParameter("typeID"));
    jassert(typePtr != nullptr);

    transformPtr = dynamic_cast      <juce::AudioParameterChoice*>    (apvts.getParameter("transformID"));
    jassert(transformPtr != nullptr);

    osPtr = dynamic_cast      <juce::AudioParameterChoice*>   (apvts.getParameter("osID"));
    jassert(osPtr != nullptr);

    auto osFilter = juce::dsp::Oversampling<SampleType>::filterHalfBandPolyphaseIIR;

    for (int i = 0; i < 5; ++i)
        overSample[i] = std::make_unique<juce::dsp::Oversampling<SampleType>>
        (2, i, osFilter, true, false);
}

template <typename SampleType>
void ProcessWrapper<SampleType>::createParameterLayout(std::vector<std::unique_ptr<RangedAudioParameter>>& params)
{
    auto freqRange = juce::NormalisableRange<float>(20.00f, 20000.00f, 0.01f, 00.198894f);
    auto gainRange = juce::NormalisableRange<float>(-30.00f, 30.00f, 0.01f, 1.00f);

    auto fString = juce::StringArray({ "Low Pass", "High Pass", "Band Pass"});
    auto tString = juce::StringArray({ "Direct Form I", "Direct Form II", "Direct Form I (t)", "Direct Form II (t)" });
    auto osString = juce::StringArray({ "1x", "2x", "4x", "8x", "16x" });

    params.push_back(std::make_unique<juce::AudioParameterBool>("ioID", "IO", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("frequencyID", "Frequency", freqRange, 632.45f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("resonanceID", "Resonance", 00.00f, 01.00f, 00.10f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("gainID", "Gain", gainRange, 00.00f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("typeID", "Type", fString, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("transformID", "Transform", tString, 3));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("osID", "Oversampling", osString, 0));
}

template <typename SampleType>
void ProcessWrapper<SampleType>::setOversampling()
{
    curOS = (int)osPtr->getIndex();
    if (curOS != prevOS)
    {
        overSamplingFactor = 1 << curOS;
        prevOS = curOS;
        biquad.reset(static_cast<SampleType>(0.0));
    }
}

template <typename SampleType>
void ProcessWrapper<SampleType>::prepare(double sampleRate, int samplesPerBlock, int numChannels)
{
    overSamplingFactor = 1 << curOS;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = numChannels;

    for (int i = 0; i < 5; ++i)
        overSample[i]->initProcessing(spec.maximumBlockSize);

    /*for (int i = 0; i < 5; ++i)
        overSample[i]->numChannels(spec.numChannels);*/

    prevOS = curOS;

    biquad.prepare(spec);
    setOversampling();
}

template <typename SampleType>
void ProcessWrapper<SampleType>::reset()
{
    biquad.reset(static_cast<SampleType>(0.0));

    for (int i = 0; i < 5; ++i)
        overSample[i]->reset();
}

template <typename SampleType>
void ProcessWrapper<SampleType>::update()
{
    setOversampling();

    biquad.setFrequency(frequencyPtr->get() / overSamplingFactor);
    biquad.setResonance(resonancePtr->get());
    biquad.setGain(gainPtr->get());

    if (typePtr->getIndex() == 0)
        biquad.setFilterType(FilterType::lowPass);
    else if (typePtr->getIndex() == 1)
        biquad.setFilterType(FilterType::highPass);
    else if (typePtr->getIndex() == 2)
        biquad.setFilterType(FilterType::bandPass);
    else
        biquad.setFilterType(FilterType::lowPass);

    if (transformPtr->getIndex() == 0)
        biquad.setTransformType(TransformationType::directFormI);
    else if (transformPtr->getIndex() == 1)
        biquad.setTransformType(TransformationType::directFormII);
    else if (transformPtr->getIndex() == 2)
        biquad.setTransformType(TransformationType::directFormItransposed);
    else if (transformPtr->getIndex() == 3)
        biquad.setTransformType(TransformationType::directFormIItransposed);
    else
        biquad.setTransformType(TransformationType::directFormIItransposed);
}

//==============================================================================
template <typename SampleType>
void ProcessWrapper<SampleType>::process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    update();

    juce::dsp::AudioBlock<SampleType> block(buffer);
    /*juce::dsp::ProcessContextReplacing<SampleType> context(block);
    
    if (ioPtr->get() == true)
        context.isBypassed = true;
    else
        context.isBypassed = false;

    biquad.process(context);*/



    juce::dsp::AudioBlock<SampleType> osBlock = overSample[curOS]->processSamplesUp(block);

    auto context = juce::dsp::ProcessContextReplacing(osBlock);

    if (ioPtr->get() == true)
        context.isBypassed = true;
    else
        context.isBypassed = false;

    biquad.process(context);

    overSample[curOS]->processSamplesDown(block);
}

//==============================================================================
template class ProcessWrapper<float>;
template class ProcessWrapper<double>;
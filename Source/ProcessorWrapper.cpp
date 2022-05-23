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

    mixPtr = dynamic_cast              <juce::AudioParameterFloat*>        (apvts.getParameter("mixID"));
    jassert(mixPtr != nullptr);

    auto osFilter = juce::dsp::Oversampling<SampleType>::filterHalfBandPolyphaseIIR;

    for (int i = 0; i < 5; ++i)
        overSample[i] = std::make_unique<juce::dsp::Oversampling<SampleType>>
        (2, i, osFilter, true, false);
}

template <typename SampleType>
void ProcessWrapper<SampleType>::createParameterLayout(std::vector<std::unique_ptr<RangedAudioParameter>>& params)
{
    auto dBMax = juce::Decibels::gainToDecibels(16.0f);
    auto dBMin = juce::Decibels::gainToDecibels(16.0f) * -1.0f;

    auto freqRange = juce::NormalisableRange<float>(20.00f, 20000.00f, 0.01f, 00.198894f);
    auto gainRange = juce::NormalisableRange<float>(dBMin, dBMax, 0.01f, 1.00f);
    auto mixRange = juce::NormalisableRange<float>(00.00f, 100.00f, 0.01f, 1.00f);

    auto fString = juce::StringArray({ "Low Pass (2)", "Low Pass (1)", "High Pass (2)", "High Pass (1)" , "Band Pass", "Band Pass (Q)", "Low Shelf (2)", "Low Shelf (1)", "Low Shelf (1c)", "High Shelf (2)", "High Shelf (1)", "High Shelf (1c)", "Peak", "Notch", "All Pass" });
    auto tString = juce::StringArray({ "Direct Form I", "Direct Form II", "Direct Form I (t)", "Direct Form II (t)" });
    auto osString = juce::StringArray({ "1x", "2x", "4x", "8x", "16x" });

    params.push_back(std::make_unique<juce::AudioParameterBool>("ioID", "IO", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("frequencyID", "Frequency", freqRange, 632.45f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("resonanceID", "Resonance", 00.00f, 01.00f, 00.10f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("gainID", "Gain", gainRange, 00.00f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("typeID", "Type", fString, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("transformID", "Transform", tString, 3));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("osID", "Oversampling", osString, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mixID", "Mix", mixRange, 100.00f));
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
        biquad.sampleRate = spec.sampleRate * overSamplingFactor;
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

    mixer.prepare(spec);
    biquad.prepare(spec);
    setOversampling();
}

template <typename SampleType>
void ProcessWrapper<SampleType>::reset()
{
    mixer.reset();
    biquad.reset(static_cast<SampleType>(0.0));

    for (int i = 0; i < 5; ++i)
        overSample[i]->reset();
}

template <typename SampleType>
void ProcessWrapper<SampleType>::update()
{
    setOversampling();

    biquad.setFrequency(frequencyPtr->get());
    biquad.setResonance(resonancePtr->get());
    biquad.setGain(gainPtr->get());

    if (typePtr->getIndex() == 0)
        biquad.setFilterType(FilterType::lowPass2);
    else if (typePtr->getIndex() == 1)
        biquad.setFilterType(FilterType::lowPass1);
    else if (typePtr->getIndex() == 2)
        biquad.setFilterType(FilterType::highPass2);
    else if (typePtr->getIndex() == 3)
        biquad.setFilterType(FilterType::highPass1);
    else if (typePtr->getIndex() == 4)
        biquad.setFilterType(FilterType::bandPass);
    else if (typePtr->getIndex() == 5)
        biquad.setFilterType(FilterType::bandPassQ);
    else if (typePtr->getIndex() == 6)
        biquad.setFilterType(FilterType::lowShelf2);
    else if (typePtr->getIndex() == 7)
        biquad.setFilterType(FilterType::lowShelf1);
    else if (typePtr->getIndex() == 8)
        biquad.setFilterType(FilterType::lowShelf1C);
    else if (typePtr->getIndex() == 9)
        biquad.setFilterType(FilterType::highShelf2);
    else if (typePtr->getIndex() == 10)
        biquad.setFilterType(FilterType::highShelf1);
    else if (typePtr->getIndex() == 11)
        biquad.setFilterType(FilterType::highShelf1C);
    else if (typePtr->getIndex() == 12)
        biquad.setFilterType(FilterType::peak);
    else if (typePtr->getIndex() == 13)
        biquad.setFilterType(FilterType::notch);
    else if (typePtr->getIndex() == 14)
        biquad.setFilterType(FilterType::allPass);
    else
        biquad.setFilterType(FilterType::lowPass2);

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

    mixer.setWetMixProportion(mixPtr->get() * 0.01f);
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

    overSample[curOS]->processSamplesDown(block);

    mixer.mixWetSamples(block);
}

//==============================================================================
template class ProcessWrapper<float>;
template class ProcessWrapper<double>;
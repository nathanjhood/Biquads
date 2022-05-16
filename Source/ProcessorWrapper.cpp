/*
  ==============================================================================

    ProcessorWrapper.cpp
    Created: 8 May 2022 9:38:17pm
    Author:  StoneyDSP

  ==============================================================================
*/

#include "ProcessorWrapper.h"

template <typename SampleType>
ProcessWrapper<SampleType>::ProcessWrapper(juce::AudioProcessorValueTreeState& apvts)
{
    ioPtr = dynamic_cast       <juce::AudioParameterBool*>    (apvts.getParameter("ioID"));
    jassert(ioPtr != nullptr);

    frequencyPtr = dynamic_cast      <juce::AudioParameterFloat*>    (apvts.getParameter("frequencyID"));
    jassert(frequencyPtr != nullptr);

    bandwidthPtr = dynamic_cast      <juce::AudioParameterFloat*>    (apvts.getParameter("bandwidthID"));
    jassert(bandwidthPtr != nullptr);

    gainPtr = dynamic_cast      <juce::AudioParameterFloat*>    (apvts.getParameter("gainID"));
    jassert(gainPtr != nullptr);

    transformPtr = dynamic_cast      <juce::AudioParameterChoice*>    (apvts.getParameter("transformID"));
    jassert(transformPtr != nullptr);
}

template <typename SampleType>
void ProcessWrapper<SampleType>::prepare(double sampleRate, int samplesPerBlock, int numChannels)
{
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = numChannels;

    biquad.prepare(spec);
};

template <typename SampleType>
void ProcessWrapper<SampleType>::reset()
{
    biquad.reset(static_cast<SampleType>(0.0));
};

template <typename SampleType>
void ProcessWrapper<SampleType>::createParameterLayout(std::vector<std::unique_ptr<RangedAudioParameter>>& params)
{
    auto freqRange = juce::NormalisableRange<float>(20.00f, 20000.00f, 0.01f, 00.198894f);
    auto gainRange = juce::NormalisableRange<float>(-30.00f, 30.00f, 0.01f, 1.00f);

    auto tString = juce::StringArray({ "Direct Form I", "Direct Form II", "Direct Form I (t)", "Direct Form II (t)" });

    params.push_back(std::make_unique<juce::AudioParameterBool>("ioID", "IO", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("frequencyID", "Frequency", freqRange, 632.45f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("bandwidthID", "Bandwidth", 00.00f, 01.00f, 00.10f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("gainID", "Gain", gainRange, 00.00f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("transformID", "Transform", tString, 3));
}

template <typename SampleType>
void ProcessWrapper<SampleType>::update()
{
    biquad.setFrequency(frequencyPtr->get());
    biquad.setResonance(bandwidthPtr->get());
    biquad.setGain(gainPtr->get());

    if (transformPtr->getIndex() == 0)
        biquad.setTransformType(TransformationType::directFormI);
    else if (transformPtr->getIndex() == 1)
        biquad.setTransformType(TransformationType::directFormII);
    else if (transformPtr->getIndex() == 2)
        biquad.setTransformType(TransformationType::directFormItransposed);
    else if (transformPtr->getIndex() == 3)
        biquad.setTransformType(TransformationType::directFormIItransposed);
}

//==============================================================================
template <typename SampleType>
void ProcessWrapper<SampleType>::process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    update();

    juce::dsp::AudioBlock<SampleType> block(buffer);
    juce::dsp::ProcessContextReplacing<SampleType> context(block);

    if (ioPtr->get() == true)
        context.isBypassed = true;
    else
        context.isBypassed = false;

    biquad.process(context);
};

//==============================================================================
template class ProcessWrapper<float>;
template class ProcessWrapper<double>;
/*
  ==============================================================================

    Biquads.h
    Created: 16 May 2022 2:54:47am
    Author:  StoneyDSP

  ==============================================================================
*/

#pragma once

#ifndef BIQUADS_H_INCLUDED
#define BIQUADS_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Calculations.h"
#include "Coefficients.h"
#include "Transforms.h"

/**
    A handy 2-pole Biquad multi-mode equalizer.
*/

template <typename SampleType>
class Biquads
{
public:

    using filterType = FilterType;
    using transformationType = TransformationType;

    //==============================================================================
    /** Constructor. */
    Biquads();

    //==============================================================================
    void setFrequency(SampleType newFreq);

    void setResonance(SampleType newRes);

    void setGain(SampleType newGain);

    void setFilterType(filterType newFiltType);

    void setTransformType(transformationType newTransformType);

    //==============================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();

    //==============================================================================
    /** Processes the input and output samples supplied in the processing context. */
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();
        //const auto len = inputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

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
                outputSamples[i] = processSample((int)channel, inputSamples[i]);
        }

    }


    //==============================================================================
    /** Processes one sample at a time on a given channel. */
    SampleType processSample(int channel, SampleType inputValue);

private:
    //==============================================================================
    void update();

    //==============================================================================
    Calculations<SampleType> calculate;
    Coefficients<SampleType> coeffs;
    Transformations<SampleType> transform;

    //==============================================================================
    double sampleRate = 44100.0;
    SampleType minFreq = 20.0;
    SampleType maxFreq = 20000.0;
    SampleType hz = 1000.0;
    SampleType q = 0.5;
    SampleType g = 0.0;

    transformationType transformType = transformationType::directFormIItransposed;
    filterType filtType = filterType::lowPass;
    

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Biquads)
};

#endif //BIQUADS_H_INCLUDED
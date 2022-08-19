/*
  ==============================================================================

    Transforms.h
    Created: 15 Jul 2022 1:55:41am
    Author:  natha

  ==============================================================================
*/

#pragma once

#ifndef TRANSFORMS_H_INCLUDED
#define TRANSFORMS_H_INCLUDED

#include <JuceHeader.h>
#include "Coefficient.h"

enum struct TransformationType
{
    directFormI = 0,
    directFormII = 1,
    directFormItransposed = 2,
    directFormIItransposed = 3
};

template <typename SampleType>
class IIRTransforms
{
    IIRTransforms();
    ~IIRTransforms();

    /** Sets the BiLinear Transform for the filter to use. See enum for available types. */
    void setTransformType(TransformationType newTransformType);

    //==========================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec spec);

    /** Resets the internal state variables of the processor. */
    void reset(SampleType initialValue = { 0.0 });

    /** Ensure that the state variables are rounded to zero if the state
    variables are denormals. This is only needed if you are doing sample
    by sample processing.*/
    void snapToZero() noexcept;

    //==========================================================================
    /** Processes the input and output samples supplied in the processing context. */
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();

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

#if JUCE_DSP_ENABLE_SNAP_TO_ZERO
        snapToZero();
#endif
    }

//protected:
    //==========================================================================
    /** Processes one sample at a time on a given channel. */
    //SampleType processSample(int channel, SampleType inputSample);

    SampleType directFormI(int channel, SampleType& inputValue);
    SampleType directFormII(int channel, SampleType& inputValue);
    SampleType directFormITransposed(int channel, SampleType& inputValue);
    SampleType directFormIITransposed(int channel, SampleType& inputValue);

    //==========================================================================
    /** Coefficient gain */
    Coefficient<SampleType> a[3], b[3];

    /** Coefficient calculation */
    Coefficient<SampleType> a_[3], b_[3];

    std::vector<SampleType> Wn_1, Wn_2, Xn_1, Xn_2, Yn_1, Yn_2;

    SampleType outputSample, loop;
    TransformationType transformType;
    double sampleRate = 48000.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IIRTransforms)
};

#endif //TRANSFORMS_H_INCLUDED

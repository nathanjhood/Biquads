/*
  ==============================================================================

    Transforms.h
    Created: 7 May 2022 4:36:01am
    Author:  StoneyDSP

  ==============================================================================
*/

#pragma once

#ifndef TRANSFORMS_H_INCLUDED
#define TRANSFORMS_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

enum class TransformationType
{
    directFormI,
    directFormII,
    directFormItransposed,
    directFormIItransposed
};

template <typename SampleType>
class Transformations
{
public:
    using directForm = TransformationType;
    //==============================================================================
    /** Constructor. */
    Transformations();

    //==============================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset(SampleType initialValue);

    /** Ensure that the state variables are rounded to zero if the state
    variables are denormals. This is only needed if you are doing sample
    by sample processing.*/
    void snapToZero() noexcept;

    //==============================================================================
    void coefficients(SampleType b0, SampleType b1, SampleType b2, SampleType a0, SampleType a1, SampleType a2);

    void setTransformType(directForm newTransformType);

    //==============================================================================
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

    //==============================================================================
    /** Processes one sample at a time on a given channel. */
    SampleType processSample(int channel, SampleType inputValue);

    SampleType directFormI(int channel, SampleType inputValue);

    SampleType directFormII(int channel, SampleType inputValue);

    SampleType directFormITransposed(int channel, SampleType inputValue);

    SampleType directFormIITransposed(int channel, SampleType inputValue);

private:

    //==============================================================================
    /** Unit-delay objects. */
    std::vector<SampleType> Wn_1;
    std::vector<SampleType> Wn_2;
    std::vector<SampleType> Xn_1;
    std::vector<SampleType> Xn_2;
    std::vector<SampleType> Yn_1;
    std::vector<SampleType> Yn_2;

    //==============================================================================
    /** Initialise the coefficient gains. */
    SampleType b0_ = 1.0 ;
    SampleType b1_ = 0.0 ;
    SampleType b2_ = 0.0 ;
    SampleType a0_ = 1.0 ;
    SampleType a1_ = 0.0 ;
    SampleType a2_ = 0.0 ;

    directForm transformType = directForm::directFormIItransposed;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Transformations)
};
#endif // TRANSFORMS_H_INCLUDED
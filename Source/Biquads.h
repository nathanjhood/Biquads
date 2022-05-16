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

enum class FilterType
{
    lowPass = 0,
    peak
};

enum class TransformationType
{
    directFormI = 0,
    directFormII = 1,
    directFormItransposed = 2,
    directFormIItransposed = 3
};

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
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset(SampleType initialValue);

    //==============================================================================
    void setFrequency(SampleType newFreq);

    void setResonance(SampleType newRes);

    void setGain(SampleType newGain);

    void setFilterType(filterType newFiltType);

    void setTransformType(transformationType newTransformType);

    //==============================================================================
    void coefficients();

    //void calculate(SampleType newHz, SampleType newQ, SampleType newG);

    //void coefficients(SampleType newOmega, SampleType newCos, SampleType newSin, SampleType newAlpha, SampleType newGain);

    //void apply(SampleType b0, SampleType b1, SampleType b2, SampleType a0, SampleType a1, SampleType a2);

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

    /** Ensure that the state variables are rounded to zero if the state
    variables are denormals. This is only needed if you are doing sample
    by sample processing.*/
    void snapToZero() noexcept;

private:

    //==============================================================================
    //SampleType omega(SampleType f)
    //{
    //    return f * ((pi * SampleType(2.0)) / sampleRate);
    //}

    //SampleType cosine(SampleType w)
    //{
    //    return std::cos(w);
    //}

    //SampleType sine(SampleType w)
    //{
    //    return std::sin(w);
    //}

    SampleType div(SampleType a, SampleType b)
    {
        return b != static_cast<SampleType> (0.0) ? (a / b) : static_cast<SampleType> (0.0);
    }

    /*SampleType b0() { return static_cast<SampleType>(b0_); };
    SampleType b1() { return static_cast<SampleType>(b1_); };
    SampleType b2() { return static_cast<SampleType>(b2_); };
    SampleType a0() { return static_cast<SampleType>(a0_); };
    SampleType a1() { return static_cast<SampleType>(a1_); };
    SampleType a2() { return static_cast<SampleType>(a2_); };*/

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
    SampleType b0 { 1.0 };
    SampleType b1 { 0.0 };
    SampleType b2 { 0.0 };
    SampleType a0 { 1.0 };
    SampleType a1 { 0.0 };
    SampleType a2 { 0.0 };

    //==============================================================================
    double sampleRate = 44100.0;
    //int numChannels = 1;
    SampleType minFreq = 20.0;
    SampleType maxFreq = 20000.0;
    SampleType hz = 1000.0, q = 0.5, g = 0.0;
    filterType filtType = filterType::lowPass;
    transformationType transformType = transformationType::directFormIItransposed;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Biquads)
};

#endif //BIQUADS_H_INCLUDED
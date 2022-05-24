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
    lowPass2,
    lowPass1,
    highPass2,
    highPass1,
    bandPass,
    bandPassQ,
    lowShelf2,
    lowShelf1,
    lowShelf1C,
    highShelf2,
    highShelf1,
    highShelf1C,
    peak,
    notch,
    allPass
};

enum class TransformationType
{
    directFormI,
    directFormII,
    directFormItransposed,
    directFormIItransposed
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
    /** Sets the centre Frequency of the filter. Range = 20..20000 */
    void setFrequency(SampleType newFreq);

    /** Sets the resonance of the filter. Range = 0..1 */
    void setResonance(SampleType newRes);

    /** Sets the centre Frequency gain of the filter. Peak and shelf modes only. */
    void setGain(SampleType newGain);

    /** Sets the type of the filter. See enum for availa ble types. */
    void setFilterType(filterType newFiltType);

    /** Sets the BiLinear Transform for the filter to use. See enum for availa ble types. */
    void setTransformType(transformationType newTransformType);

    //==============================================================================
    /** Sets the length of the ramp used for smoothing parameter changes. */
    void setRampDurationSeconds(double newDurationSeconds) noexcept;

    /** Returns the ramp duration in seconds. */
    double getRampDurationSeconds() const noexcept;

    /** Returns true if the current value is currently being interpolated. */
    bool isSmoothing() const noexcept;

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
    /** Processes the input and output samples supplied in the processing context. */
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();
        const auto len = inputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

        if (context.isBypassed)
        {
            frq.skip(static_cast<int> (len));
            res.skip(static_cast<int> (len));
            lev.skip(static_cast<int> (len));

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

    double sampleRate = 44100.0, rampDurationSeconds = 0.00005;

private:
    //==============================================================================
    void coefficients();

    SampleType directFormI(int channel, SampleType inputValue);

    SampleType directFormII(int channel, SampleType inputValue);

    SampleType directFormITransposed(int channel, SampleType inputValue);

    SampleType directFormIITransposed(int channel, SampleType inputValue);

    //==============================================================================
    SampleType b0() { return static_cast<SampleType>(b0_); }
    SampleType b1() { return static_cast<SampleType>(b1_); }
    SampleType b2() { return static_cast<SampleType>(b2_); }
    SampleType a0() { return static_cast<SampleType>(a0_); }
    SampleType a1() { return static_cast<SampleType>(a1_); }
    SampleType a2() { return static_cast<SampleType>(a2_); }

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
    SampleType b0_ = 1.0;
    SampleType b1_ = 0.0;
    SampleType b2_ = 0.0;
    SampleType a0_ = 1.0;
    SampleType a1_ = 0.0;
    SampleType a2_ = 0.0;

    //==============================================================================
    /** Parameter Smoothers. */
    juce::SmoothedValue<SampleType, juce::ValueSmoothingTypes::Multiplicative> frq;
    juce::SmoothedValue<SampleType, juce::ValueSmoothingTypes::Linear> res;
    juce::SmoothedValue<SampleType, juce::ValueSmoothingTypes::Linear> lev;

    //==============================================================================
    /** Initialise the parameters. */
    SampleType minFreq = 20.0;
    SampleType maxFreq = 20000.0;
    SampleType hz = 1000.0;
    SampleType q = 0.5;
    SampleType g = 0.0;

    filterType filtType = filterType::lowPass2;
    transformationType transformType = transformationType::directFormIItransposed;
    
    //==============================================================================
    /** Initialise constants. */
    const SampleType zero = (0.0);
    const SampleType one = (1.0);
    const SampleType two = (2.0);
    const SampleType minusOne = (-1.0);
    const SampleType minusTwo = (-2.0);
    const SampleType pi = (juce::MathConstants<SampleType>::pi);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Biquads)
};

#endif //BIQUADS_H_INCLUDED
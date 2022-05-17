/*
  ==============================================================================

    Coefficients.h
    Created: 13 May 2022 8:29:03pm
    Author:  StoneyDSP

  ==============================================================================
*/

#pragma once

#ifndef COEFFICIENTS_H_INCLUDED
#define COEFFICIENTS_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"


enum class FilterType
{
    lowPass = 0,
    peak
};

template <typename SampleType>
class Coefficients
{
public:
    using filterType = FilterType;
    //==============================================================================
    /** Constructor. */
    Coefficients();

    //==============================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();

    void processCoeffs(SampleType newOmega, SampleType newCos, SampleType newSin, SampleType newAlpha, SampleType newGain);

    void peakFilter(SampleType newOmega, SampleType newCos, SampleType newSin, SampleType newAlpha, SampleType newGain);

    //==============================================================================
    SampleType b0() { return static_cast<SampleType>(b0_); };
    SampleType b1() { return static_cast<SampleType>(b1_); };
    SampleType b2() { return static_cast<SampleType>(b2_); };
    SampleType a0() { return static_cast<SampleType>(a0_); };
    SampleType a1() { return static_cast<SampleType>(a1_); };
    SampleType a2() { return static_cast<SampleType>(a2_); };

private:
    //==============================================================================
    double sampleRate = 44100.0;
    const SampleType pi = static_cast<SampleType>(juce::MathConstants<SampleType>::pi);

    //==============================================================================
    /** Initialise the coefficient gains. */
    SampleType b0_ = 1.0;
    SampleType b1_ = 0.0;
    SampleType b2_ = 0.0;
    SampleType a0_ = 1.0;
    SampleType a1_ = 0.0;
    SampleType a2_ = 0.0;

    FilterType filter = FilterType::lowPass;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Coefficients)

};

#endif // COEFFICIENTS_H_INCLUDED
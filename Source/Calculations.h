/*
  ==============================================================================

    Calculations.h
    Created: 16 May 2022 2:19:42am
    Author:  StoneyDSP

  ==============================================================================
*/

#pragma once

#ifndef CALCULATIONS_H_INCLUDED
#define CALCULATIONS_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

template <typename SampleType>
class Calculations
{
public:
    using filterType = FilterType;
    //==============================================================================
    /** Constructor. */
    Calculations();

    //==============================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();

    void calc(SampleType newFreq, SampleType newRes, SampleType newdB);

private:
    //==============================================================================
    double sampleRate = 44100.0;
    const SampleType pi = static_cast<SampleType>(juce::MathConstants<SampleType>::pi);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Calculations)
};

#endif // CALCULATIONS_H_INCLUDED
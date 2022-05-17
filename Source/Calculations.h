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
    //==============================================================================
    /** Constructor. */
    Calculations();

    //==============================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();

    //==============================================================================
    void setFrequency(SampleType newFreq);

    void setResonance(SampleType newRes);

    void setGain(SampleType newGain);

    //==============================================================================
    SampleType omega()      { return static_cast<SampleType>(omega_)    ; };
    SampleType sine()       { return static_cast<SampleType>(sine_)     ; };
    SampleType cosine()     { return static_cast<SampleType>(cosine_)   ; };
    SampleType tan()        { return static_cast<SampleType>(tan_)      ; };
    SampleType alpha()      { return static_cast<SampleType>(alpha_)    ; };
    SampleType a()          { return static_cast<SampleType>(a_)        ; };

private:

    void calc();
    //==============================================================================
    
    
    SampleType minFreq = 20.0;
    SampleType maxFreq = 20000.0;

    SampleType hz = 1000.0, q = 0.5, g = 0.0;

    SampleType omega_;
    SampleType sine_;
    SampleType cosine_;
    SampleType tan_;
    SampleType alpha_;
    SampleType a_;

    double sampleRate = 44100.0;
    const SampleType pi = static_cast<SampleType>(juce::MathConstants<SampleType>::pi);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Calculations)
};

#endif // CALCULATIONS_H_INCLUDED
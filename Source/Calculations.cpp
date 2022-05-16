/*
  ==============================================================================

    Calculations.cpp
    Created: 16 May 2022 2:19:42am
    Author:  StoneyDSP

  ==============================================================================
*/

#include "Calculations.h"

template <typename SampleType>
Calculations<SampleType>::Calculations()
{

}

template <typename SampleType>
void Calculations<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);

    sampleRate = spec.sampleRate;
}

template <typename SampleType>
void Calculations<SampleType>::reset()
{

}

template <typename SampleType>
void Calculations<SampleType>::calc(SampleType newFreq, SampleType newRes, SampleType newdB)
{
    jassert(SampleType(20.0) <= newFreq && newFreq <= SampleType(20000.0));

    const SampleType minFreq = static_cast <SampleType>(sampleRate) / SampleType(24576.0);
    const SampleType maxFreq = static_cast <SampleType>(sampleRate) / SampleType(2.125);

    auto hz = jlimit(minFreq, maxFreq, newFreq);

    auto omega = hz * ((pi * SampleType(2.0)) / sampleRate);
    auto cos = std::cos(omega);
    auto sin = std::sin(omega);
    auto tan = sin / cos;
    auto alpha = (sin * (SampleType(1.0) - newRes));
    auto gain = std::pow(SampleType(10.0), (newdB / SampleType(40.0)));
}
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

    sampleRate = static_cast <SampleType>(spec.sampleRate);

    minFreq = static_cast <SampleType>(sampleRate) / static_cast <SampleType>(24576.0);
    maxFreq = static_cast <SampleType>(sampleRate) / static_cast <SampleType>(2.125);

    jassert(static_cast <SampleType>(20.0) >= minFreq && minFreq <= static_cast <SampleType>(20000.0));
    jassert(static_cast <SampleType>(20.0) <= maxFreq && maxFreq >= static_cast <SampleType>(20000.0));
}

template <typename SampleType>
void Calculations<SampleType>::reset()
{

}

template <typename SampleType>
void Calculations<SampleType>::setFrequency(SampleType newFreq)
{
    jassert(SampleType(20.0) <= newFreq && newFreq <= SampleType(20000.0));

    hz = jlimit(minFreq, maxFreq, newFreq);
}

template <typename SampleType>
void Calculations<SampleType>::setResonance(SampleType newRes)
{
    jassert(SampleType(0.0) <= newRes && newRes <= SampleType(1.0));

    q = (jlimit(SampleType(0.0), SampleType(1.0), newRes));
}

template <typename SampleType>
void Calculations<SampleType>::setGain(SampleType newGain)
{
    g = newGain;
}

template <typename SampleType>
void Calculations<SampleType>::calc()
{
    omega_ = static_cast <SampleType>(hz * ((pi * static_cast <SampleType>(2.0)) / sampleRate));
    cosine_ = static_cast <SampleType>(std::cos(omega_));
    sine_ = static_cast <SampleType>(std::sin(omega_));
    tan_ = static_cast <SampleType>(sine_ / cosine_);
    alpha_ = static_cast <SampleType>(sine_ * (SampleType(1.0) - q));
    a_ = static_cast <SampleType>(std::pow(static_cast <SampleType>(10.0), (g / static_cast <SampleType>(40.0))));
}

template class Calculations<float>;
template class Calculations<double>;
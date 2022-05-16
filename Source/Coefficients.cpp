/*
  ==============================================================================

    Biquads.cpp
    Created: 13 May 2022 8:29:03pm
    Author:  StoneyDSP

  ==============================================================================
*/

#include "Coefficients.h"

//==============================================================================
template <typename SampleType>
Coefficients<SampleType>::Coefficients()
{
    b0_ = SampleType(1.0);
    b1_ = SampleType(0.0);
    b2_ = SampleType(0.0);
    a0_ = SampleType(1.0);
    a1_ = SampleType(0.0);
    a2_ = SampleType(0.0);
}

template <typename SampleType>
void Coefficients<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);

    sampleRate = spec.sampleRate;
}

template <typename SampleType>
void Coefficients<SampleType>::reset()
{

}

template <typename SampleType>
void Coefficients<SampleType>::processCoeffs(SampleType newOmega, SampleType newCos, SampleType newSin, SampleType newAlpha, SampleType newGain)
{
    peakFilter(newOmega, newCos, newSin, newAlpha, newGain);
}

template <typename SampleType>
void Coefficients<SampleType>::peakFilter(SampleType newOmega, SampleType newCos, SampleType newSin, SampleType newAlpha, SampleType newGain)
{
    juce::ignoreUnused(newOmega);
    juce::ignoreUnused(newSin);

    auto alphaMulGain = newAlpha * newGain;
    auto alphaDivGain = newAlpha / newGain;
    auto cosMinTwo = newCos * SampleType(-2.0);

    const SampleType one = static_cast<SampleType>(1.0);

    b0_ = (alphaMulGain + one);
    b1_ = (cosMinTwo);
    b2_ = (one - alphaMulGain);
    a0_ = (alphaDivGain + one);
    a1_ = (cosMinTwo);
    a2_ = (one - alphaDivGain);
}

template class Coefficients<float>;
template class Coefficients<double>;
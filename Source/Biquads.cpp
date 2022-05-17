/*
  ==============================================================================

    Biquads.cpp
    Created: 16 May 2022 2:54:47am
    Author:  StoneyDSP

  ==============================================================================
*/

#include "Biquads.h"

//==============================================================================
template <typename SampleType>
Biquads<SampleType>::Biquads()
{
    update();
}

//==============================================================================
template <typename SampleType>
void Biquads<SampleType>::setFrequency(SampleType newFreq)
{
    jassert(SampleType(20.0) <= newFreq && newFreq <= SampleType(20000.0));

    hz = (jlimit(minFreq, maxFreq, newFreq));
    update();
}

template <typename SampleType>
void Biquads<SampleType>::setResonance(SampleType newRes)
{
    jassert(SampleType(0.0) <= newRes && newRes <= SampleType(1.0));

    q = (jlimit(SampleType(0.0), SampleType(1.0), newRes));
    update();
}

template <typename SampleType>
void Biquads<SampleType>::setGain(SampleType newGain)
{
    g = newGain;
    update();
}

template <typename SampleType>
void Biquads<SampleType>::setFilterType(filterType newFiltType)
{
    if (filtType != newFiltType)
    {
        filtType = newFiltType;
        //reset(static_cast<SampleType>(0.0));
        //coefficients();
    }
}

template <typename SampleType>
void Biquads<SampleType>::setTransformType(transformationType newTransformType)
{
    if (transformType != newTransformType)
    {
        transformType = newTransformType;
        //reset(static_cast<SampleType>(0.0));
        //coefficients();
    }
}

//==============================================================================
template <typename SampleType>
void Biquads<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    sampleRate = spec.sampleRate;

    calculate.prepare(spec);
    coeffs.prepare(spec);
    transform.prepare(spec);

    update();
    reset();

    minFreq = static_cast <SampleType>(sampleRate) / static_cast <SampleType>(24576.0);
    maxFreq = static_cast <SampleType>(sampleRate) / static_cast <SampleType>(2.125);

    jassert(static_cast <SampleType>(20.0) >= minFreq && minFreq <= static_cast <SampleType>(20000.0));
    jassert(static_cast <SampleType>(20.0) <= maxFreq && maxFreq >= static_cast <SampleType>(20000.0));
}

template <typename SampleType>
void Biquads<SampleType>::reset()
{
    calculate.reset();
    coeffs.reset();
    transform.reset(static_cast<SampleType>(0.0));
}

////==============================================================================
//
//template <typename SampleType>
//void Biquads<SampleType>::coefficients()
//{
//    const SampleType pi = static_cast<SampleType>(juce::MathConstants<SampleType>::pi);
//    const SampleType one = static_cast<SampleType>(1.0);
//    
//    auto omega = hz * ((pi * 2.0) / sampleRate);
//    auto cos = std::cos(omega);
//    auto sin = std::sin(omega);
//    auto tan = sin / cos;
//    auto alpha = (sin * (SampleType(1.0) - q));
//    auto a = std::pow(SampleType(10.0), (g / SampleType(40.0)));
//
//    //LP2 with resonance...
//    auto oneMinCos = one - cos;
//    auto cosTwo = oneMinCos / SampleType(2.0);
//    auto cosMinTwo = cos * SampleType(-2.0);
//
//    auto b0_ = (cosTwo);
//    auto b1_ = (oneMinCos);
//    auto b2_ = (cosTwo);
//    auto a0_ = (alpha + one);
//    auto a1_ = (cosMinTwo);
//    auto a2_ = (one - alpha);
//
//    // Apply...
//    a0 = (static_cast <SampleType>(SampleType(1.0) / a0_));
//    a1 = (static_cast <SampleType>((a1_ * a0) * SampleType(-1.0)));
//    a2 = (static_cast <SampleType>((a2_ * a0) * SampleType(-1.0)));
//    b0 = (static_cast <SampleType>(b0_ * a0));
//    b1 = (static_cast <SampleType>(b1_ * a0));
//    b2 = (static_cast <SampleType>(b2_ * a0));
//
//
//}

//template <typename SampleType>
//void Biquads<SampleType>::coefficients(SampleType newOmega, SampleType newCos, SampleType newSin, SampleType newAlpha, SampleType newGain)
//{
//    //LP2 with resonance...
//    juce::ignoreUnused(newOmega);
//    juce::ignoreUnused(newSin);
//
//    const SampleType one = static_cast<SampleType>(1.0);
//
//    auto alphaMulGain = newAlpha * newGain;
//    auto alphaDivGain = newAlpha / newGain;
//    auto cosMinOne = one - newCos;
//    auto cosMinTwo = newCos * SampleType(-2.0);
//    auto divTwo = cosMinOne / SampleType(2.0);
//
//    
//
//    auto b0_ = (divTwo);
//    auto b1_ = (cosMinOne);
//    auto b2_ = (divTwo);
//    auto a0_ = (alphaDivGain + one);
//    auto a1_ = (cosMinTwo);
//    auto a2_ = (one - alphaDivGain);
//
//    // Apply...
//    a0 = (static_cast <SampleType>(div(one, a0_)));
//    a1 = (static_cast <SampleType>((a1_ * a0) * SampleType(-1.0)));
//    a2 = (static_cast <SampleType>((a2_ * a0) * SampleType(-1.0)));
//    b0 = (static_cast <SampleType>(b0_ * a0));
//    b1 = (static_cast <SampleType>(b1_ * a0));
//    b2 = (static_cast <SampleType>(b2_ * a0));
//
//
//}

template <typename SampleType>
SampleType Biquads<SampleType>::processSample(int channel, SampleType inputValue)
{
    
    transform.processSample(channel, inputValue);

    return inputValue;
}

template <typename SampleType>
void Biquads<SampleType>::update()
{
    //transform.coefficients();
    //transform.setTransformType();
}

//==============================================================================
template class Biquads<float>;
template class Biquads<double>;
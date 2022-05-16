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
    reset(static_cast<SampleType>(0.0));
}

//==============================================================================
template <typename SampleType>
void Biquads<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    sampleRate = spec.sampleRate;

    Wn_1.resize(spec.numChannels);
    Wn_2.resize(spec.numChannels);
    Xn_1.resize(spec.numChannels);
    Xn_2.resize(spec.numChannels);
    Yn_1.resize(spec.numChannels);
    Yn_2.resize(spec.numChannels);

    reset(static_cast<SampleType>(0.0));

    minFreq = static_cast <SampleType>(sampleRate) / SampleType(24576.0);
    maxFreq = static_cast <SampleType>(sampleRate) / SampleType(2.125);

    jassert(SampleType(20.0) >= minFreq && minFreq <= SampleType(20000.0));
    jassert(SampleType(20.0) <= maxFreq && maxFreq >= SampleType(20000.0));
}

template <typename SampleType>
void Biquads<SampleType>::reset(SampleType initialValue)
{
    for (auto& wn_1 : Wn_1)
        wn_1 = initialValue;

    for (auto& wn_2 : Wn_2)
        wn_2 = initialValue;

    for (auto& xn_1 : Xn_1)
        xn_1 = initialValue;

    for (auto& xn_2 : Xn_2)
        xn_2 = initialValue;

    for (auto& yn_1 : Yn_1)
        yn_1 = initialValue;

    for (auto& yn_2 : Yn_2)
        yn_2 = initialValue;
}

//==============================================================================
template <typename SampleType>
void Biquads<SampleType>::setFrequency(SampleType newFreq)
{
    jassert(SampleType(20.0) <= newFreq && newFreq <= SampleType(20000.0));

    hz = (jlimit(minFreq, maxFreq, newFreq));
    coefficients();
}

template <typename SampleType>
void Biquads<SampleType>::setResonance(SampleType newRes)
{
    jassert(SampleType(0.0) <= newRes && newRes <= SampleType(1.0));

    q = (jlimit(SampleType(0.0), SampleType(1.0), newRes));
    coefficients();
}

template <typename SampleType>
void Biquads<SampleType>::setGain(SampleType newGain)
{
    g = newGain;
    coefficients();
}

template <typename SampleType>
void Biquads<SampleType>::setFilterType(filterType newFiltType)
{
    if (filtType != newFiltType)
    {
        filtType = newFiltType;
        reset(static_cast<SampleType>(0.0));
        coefficients();
    }
}

template <typename SampleType>
void Biquads<SampleType>::setTransformType(transformationType newTransformType)
{
    if (transformType != newTransformType)
    {
        transformType = newTransformType;
        reset(static_cast<SampleType>(0.0));
        coefficients();
    }
}

//==============================================================================

template <typename SampleType>
void Biquads<SampleType>::coefficients()
{
    const SampleType pi = static_cast<SampleType>(juce::MathConstants<SampleType>::pi);
    const SampleType one = static_cast<SampleType>(1.0);
    
    auto omega = hz * ((pi * 2.0) / sampleRate);
    auto cos = std::cos(omega);
    auto sin = std::sin(omega);
    auto tan = sin / cos;
    auto alpha = (sin * (SampleType(1.0) - q));
    auto a = std::pow(SampleType(10.0), (g / SampleType(40.0)));

    //LP2 with resonance...
    auto oneMinCos = one - cos;
    auto cosTwo = oneMinCos / SampleType(2.0);
    auto cosMinTwo = cos * SampleType(-2.0);

    auto b0_ = (cosTwo);
    auto b1_ = (oneMinCos);
    auto b2_ = (cosTwo);
    auto a0_ = (alpha + one);
    auto a1_ = (cosMinTwo);
    auto a2_ = (one - alpha);

    // Apply...
    a0 = (static_cast <SampleType>(SampleType(1.0) / a0_));
    a1 = (static_cast <SampleType>((a1_ * a0) * SampleType(-1.0)));
    a2 = (static_cast <SampleType>((a2_ * a0) * SampleType(-1.0)));
    b0 = (static_cast <SampleType>(b0_ * a0));
    b1 = (static_cast <SampleType>(b1_ * a0));
    b2 = (static_cast <SampleType>(b2_ * a0));


}
 
//template <typename SampleType>
//void Biquads<SampleType>::calculate(SampleType newFreq, SampleType newRes, SampleType newdB)
//{
//    SampleType pi = static_cast<SampleType>(juce::MathConstants<SampleType>::pi);
//
//    auto omega = hz * ((pi * 2.0) / sampleRate);
//    auto cos = std::cos(omega);
//    auto sin = std::sin(omega);
//    auto tan = sin / cos;
//    auto alpha = (sin * (SampleType(1.0) - q));
//    auto a = std::pow(SampleType(10.0), (g / SampleType(40.0)));
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

//template <typename SampleType>
//void Biquads<SampleType>::apply(SampleType b0, SampleType b1, SampleType b2, SampleType a0, SampleType a1, SampleType a2)
//{
//    a0_ = (static_cast <SampleType>(1.0) / a0);
//    a1_ = (static_cast <SampleType>((a1 * a0_) * SampleType(-1.0)));
//    a2_ = (static_cast <SampleType>((a2 * a0_) * SampleType(-1.0)));
//    b0_ = (static_cast <SampleType>(b0 * a0_));
//    b1_ = (static_cast <SampleType>(b1 * a0_));
//    b2_ = (static_cast <SampleType>(b2 * a0_));
//}

template <typename SampleType>
SampleType Biquads<SampleType>::processSample(int channel, SampleType inputValue)
{
    jassert(isPositiveAndBelow(channel, Wn_1.size()));
    jassert(isPositiveAndBelow(channel, Wn_2.size()));
    jassert(isPositiveAndBelow(channel, Xn_1.size()));
    jassert(isPositiveAndBelow(channel, Xn_2.size()));
    jassert(isPositiveAndBelow(channel, Yn_1.size()));
    jassert(isPositiveAndBelow(channel, Yn_1.size()));

    if (transformType == TransformationType::directFormI)
        inputValue = directFormI(channel, inputValue);
    else if (transformType == TransformationType::directFormII)
        inputValue = directFormII(channel, inputValue);
    else if (transformType == TransformationType::directFormItransposed)
        inputValue = directFormITransposed(channel, inputValue);
    else if (transformType == TransformationType::directFormIItransposed)
        inputValue = directFormIITransposed(channel, inputValue);

    return inputValue;
}

template <typename SampleType>
SampleType Biquads<SampleType>::directFormI(int channel, SampleType inputValue)
{
    SampleType Xn = inputValue;

    SampleType Yn = ((Xn * b0) + (Xn_1[(size_t)channel] * b1) + (Xn_2[(size_t)channel] * b2) + (Yn_1[(size_t)channel] * a1) + (Yn_2[(size_t)channel] * a2));


    Xn_2[(size_t)channel] = Xn_1[(size_t)channel];
    Xn_1[(size_t)channel] = Xn;

    Yn_2[(size_t)channel] = Yn_1[(size_t)channel];
    Yn_1[(size_t)channel] = Yn;

    return Yn;
}

template <typename SampleType>
SampleType Biquads<SampleType>::directFormII(int channel, SampleType inputValue)
{
    SampleType Xn = inputValue;

    SampleType Wn = (Xn + ((Wn_1[(size_t)channel] * a1) + (Wn_2[(size_t)channel] * a2)));
    SampleType Yn = ((Wn * b0) + (Wn_1[(size_t)channel] * b1) + (Wn_2[(size_t)channel] * b2));

    Wn_2[(size_t)channel] = Wn_1[(size_t)channel];
    Wn_1[(size_t)channel] = Wn;

    return Yn;
}

template <typename SampleType>
SampleType Biquads<SampleType>::directFormITransposed(int channel, SampleType inputValue)
{
    SampleType Xn = inputValue;
    SampleType Wn = (Xn + Wn_2[(size_t)channel]);
    SampleType Yn = ((Wn * b0) + Xn_2[(size_t)channel]);

    Xn_2[(size_t)channel] = ((Wn * b1) + Xn_1[(size_t)channel]);
    Xn_1[(size_t)channel] = (Wn * b2);

    Wn_2[(size_t)channel] = ((Wn * a1) + Wn_1[(size_t)channel]);
    Wn_1[(size_t)channel] = (Wn * a2);

    return Yn;
}

template <typename SampleType>
SampleType Biquads<SampleType>::directFormIITransposed(int channel, SampleType inputValue)
{
    SampleType Xn = inputValue;

    SampleType Yn = ((Xn * b0) + (Xn_2[(size_t)channel]));

    Xn_2[(size_t)channel] = ((Xn * b1) + (Xn_1[(size_t)channel]) + (Yn * a1));
    Xn_1[(size_t)channel] = ((Xn * b2) + (Yn * a2));

    return Yn;
}

template <typename SampleType>
void Biquads<SampleType>::snapToZero() noexcept
{
    for (auto& wn_1 : Wn_1)
        juce::dsp::util::snapToZero(wn_1);

    for (auto& wn_2 : Wn_2)
        juce::dsp::util::snapToZero(wn_2);

    for (auto& xn_1 : Xn_1)
        juce::dsp::util::snapToZero(xn_1);

    for (auto& xn_2 : Xn_2)
        juce::dsp::util::snapToZero(xn_2);

    for (auto& yn_1 : Yn_1)
        juce::dsp::util::snapToZero(yn_1);

    for (auto& yn_2 : Yn_2)
        juce::dsp::util::snapToZero(yn_2);
}

//==============================================================================
template class Biquads<float>;
template class Biquads<double>;
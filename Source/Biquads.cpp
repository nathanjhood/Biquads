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
    b0_ = SampleType(1.0);
    b1_ = SampleType(0.0);
    b2_ = SampleType(0.0);
    a0_ = SampleType(1.0);
    a1_ = SampleType(0.0);
    a2_ = SampleType(0.0);

    update();
    reset(static_cast<SampleType>(0.0));
}

//==============================================================================
template <typename SampleType>
void Biquads<SampleType>::setFrequency(SampleType newFreq)
{
    jassert(static_cast<SampleType>(20.0) <= newFreq && newFreq <= static_cast<SampleType>(20000.0));

    hz = static_cast<SampleType>(jlimit(minFreq, maxFreq, newFreq));
    update();
}

template <typename SampleType>
void Biquads<SampleType>::setResonance(SampleType newRes)
{
    jassert(static_cast<SampleType>(0.0) <= newRes && newRes <= static_cast<SampleType>(1.0));

    q = static_cast<SampleType>(jlimit(SampleType(0.0), SampleType(1.0), newRes));
    update();
}

template <typename SampleType>
void Biquads<SampleType>::setGain(SampleType newGain)
{
    g = static_cast<SampleType>(newGain);
    update();
}

template <typename SampleType>
void Biquads<SampleType>::setFilterType(filterType newFiltType)
{
    if (filtType != newFiltType)
    {
        filtType = newFiltType;
        reset(static_cast<SampleType>(0.0));
        update();
    }
}

template <typename SampleType>
void Biquads<SampleType>::setTransformType(transformationType newTransformType)
{
    if (transformType != newTransformType)
    {
        transformType = newTransformType;
        reset(static_cast<SampleType>(0.0));
        update();
    }
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

    update();
    reset(static_cast<SampleType>(0.0));

    minFreq = static_cast <SampleType>(sampleRate) / static_cast <SampleType>(24576.0);
    maxFreq = static_cast <SampleType>(sampleRate) / static_cast <SampleType>(2.125);

    jassert(static_cast <SampleType>(20.0) >= minFreq && minFreq <= static_cast <SampleType>(20000.0));
    jassert(static_cast <SampleType>(20.0) <= maxFreq && maxFreq >= static_cast <SampleType>(20000.0));
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

    b0_ = SampleType(1.0);
    b1_ = SampleType(0.0);
    b2_ = SampleType(0.0);
    a0_ = SampleType(1.0);
    a1_ = SampleType(0.0);
    a2_ = SampleType(0.0);
}

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

    SampleType Yn = ((Xn * b0_) + (Xn_1[(size_t)channel] * b1_) + (Xn_2[(size_t)channel] * b2_) + (Yn_1[(size_t)channel] * a1_) + (Yn_2[(size_t)channel] * a2_));


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

    SampleType Wn = (Xn + ((Wn_1[(size_t)channel] * a1_) + (Wn_2[(size_t)channel] * a2_)));
    SampleType Yn = ((Wn * b0_) + (Wn_1[(size_t)channel] * b1_) + (Wn_2[(size_t)channel] * b2_));

    Wn_2[(size_t)channel] = Wn_1[(size_t)channel];
    Wn_1[(size_t)channel] = Wn;

    return Yn;
}

template <typename SampleType>
SampleType Biquads<SampleType>::directFormITransposed(int channel, SampleType inputValue)
{
    SampleType Xn = inputValue;
    SampleType Wn = (Xn + Wn_2[(size_t)channel]);
    SampleType Yn = ((Wn * b0_) + Xn_2[(size_t)channel]);

    Xn_2[(size_t)channel] = ((Wn * b1_) + Xn_1[(size_t)channel]);
    Xn_1[(size_t)channel] = (Wn * b2_);

    Wn_2[(size_t)channel] = ((Wn * a1_) + Wn_1[(size_t)channel]);
    Wn_1[(size_t)channel] = (Wn * a2_);

    return Yn;
}

template <typename SampleType>
SampleType Biquads<SampleType>::directFormIITransposed(int channel, SampleType inputValue)
{
    SampleType Xn = inputValue;

    SampleType Yn = ((Xn * b0_) + (Xn_2[(size_t)channel]));

    Xn_2[(size_t)channel] = ((Xn * b1_) + (Xn_1[(size_t)channel]) + (Yn * a1_));
    Xn_1[(size_t)channel] = ((Xn * b2_) + (Yn * a2_));

    return Yn;
}

template <typename SampleType>
void Biquads<SampleType>::update()
{
    auto omega = static_cast <SampleType>(hz * ((pi * static_cast <SampleType>(2.0)) / sampleRate));
    auto cos = static_cast <SampleType>(std::cos(omega));
    auto sin = static_cast <SampleType>(std::sin(omega));
    auto tan = static_cast <SampleType>(sin / cos);
    auto alpha = static_cast <SampleType>(sin * (static_cast <SampleType>(1.0) - q));
    auto a = static_cast <SampleType>(std::pow(static_cast <SampleType>(10.0), (g / static_cast <SampleType>(40.0))));

    coefficients(omega, cos, sin, tan, alpha, a);
}

template <typename SampleType>
void Biquads<SampleType>::coefficients(SampleType newOmega, SampleType newCos, SampleType newSin, SampleType newTan, SampleType newAlpha, SampleType newGain)
{
    juce::ignoreUnused(newOmega);
    //juce::ignoreUnused(newCos);
    juce::ignoreUnused(newSin);
    juce::ignoreUnused(newTan);
    //juce::ignoreUnused(newAlpha);
    //juce::ignoreUnused(newGain);

    SampleType alphaMulGain = static_cast <SampleType>(newAlpha * newGain);
    SampleType alphaDivGain = static_cast <SampleType>(newAlpha / newGain);
    SampleType cosMinOne = static_cast <SampleType>(static_cast <SampleType>(1.0) - newCos);
    SampleType cosMinTwo = static_cast <SampleType>(newCos * static_cast <SampleType>(-2.0));
    SampleType divTwo = static_cast <SampleType>(cosMinOne / static_cast <SampleType>(2.0));

    switch (filtType)
    {
        case filterType::lowPass:

            b0_ = static_cast <SampleType>(divTwo);
            b1_ = static_cast <SampleType>(cosMinOne);
            b2_ = static_cast <SampleType>(divTwo);
            a0_ = static_cast <SampleType>(alphaDivGain + static_cast <SampleType>(1.0));
            a1_ = static_cast <SampleType>(cosMinTwo);
            a2_ = static_cast <SampleType>(static_cast <SampleType>(1.0) - alphaDivGain);

            break;


        default:

            b0_ = static_cast <SampleType>(divTwo);
            b1_ = static_cast <SampleType>(cosMinOne);
            b2_ = static_cast <SampleType>(divTwo);
            a0_ = static_cast <SampleType>(alphaDivGain + static_cast <SampleType>(1.0));
            a1_ = static_cast <SampleType>(cosMinTwo);
            a2_ = static_cast <SampleType>(static_cast <SampleType>(1.0) - alphaDivGain);

            break;

    }
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
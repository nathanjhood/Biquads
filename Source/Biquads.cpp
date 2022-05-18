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
void Biquads<SampleType>::setFrequency(SampleType newFreq)
{
    jassert(static_cast<SampleType>(20.0) <= newFreq && newFreq <= static_cast<SampleType>(20000.0));

    hz = static_cast<SampleType>(jlimit(minFreq, maxFreq, newFreq));
    coefficients();
}

template <typename SampleType>
void Biquads<SampleType>::setResonance(SampleType newRes)
{
    jassert(static_cast<SampleType>(0.0) <= newRes && newRes <= static_cast<SampleType>(1.0));

    q = static_cast<SampleType>(jlimit(SampleType(0.0), SampleType(1.0), newRes));
    coefficients();
}

template <typename SampleType>
void Biquads<SampleType>::setGain(SampleType newGain)
{
    g = static_cast<SampleType>(newGain);
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

    coefficients();
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

    coefficients();
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
void Biquads<SampleType>::coefficients()
{
    SampleType omega = static_cast <SampleType>(hz * ((pi * static_cast <SampleType>(2.0)) / sampleRate));
    SampleType cos = static_cast <SampleType>(std::cos(omega));
    SampleType sin = static_cast <SampleType>(std::sin(omega));
    SampleType tan = static_cast <SampleType>(sin / cos);
    SampleType alpha = static_cast <SampleType>(sin * (static_cast <SampleType>(1.0) - q));
    SampleType a = static_cast <SampleType>(std::pow(static_cast <SampleType>(10.0), (g / static_cast <SampleType>(40.0))));

    juce::ignoreUnused(tan);

    SampleType b0 = SampleType(1.0);
    SampleType b1 = SampleType(0.0);
    SampleType b2 = SampleType(0.0);
    SampleType a0 = SampleType(1.0);
    SampleType a1 = SampleType(0.0);
    SampleType a2 = SampleType(0.0);

    switch (filtType)
    {
        case filterType::lowPass:

            b0 = (SampleType(1.0) - cos) / SampleType(2.0);
            b1 = SampleType(1.0) - cos;
            b2 = (SampleType(1.0) - cos) / SampleType(2.0);
            a0 = alpha + SampleType(1.0);
            a1 = cos * SampleType(-2.0);
            a2 = SampleType(1.0) - alpha;

            break;


        case filterType::highPass:

            b0 = (SampleType(1.0) + cos) / SampleType(2.0);
            b1 = (SampleType(1.0) + cos) * SampleType(-1.0);
            b2 = (SampleType(1.0) + cos) / SampleType(2.0);
            a0 = alpha + SampleType(1.0);
            a1 = cos * SampleType(-2.0);
            a2 = SampleType(1.0) - alpha;

            break;

        case filterType::bandPass:

            b0 = sin / SampleType(2.0);
            b1 = SampleType(0.0);
            b2 = (sin / SampleType(2.0)) * SampleType(-1.0);
            a0 = alpha + SampleType(1.0);
            a1 = cos * SampleType(-2.0);
            a2 = SampleType(1.0) - alpha;

            break;

        case filterType::peak:

            b0 = SampleType(1.0) + (alpha * a);
            b1 = SampleType(-2.0) * cos;
            b2 = SampleType(1.0) - (alpha * a);
            a0 = SampleType(1.0) + (alpha / a);
            a1 = SampleType(-2.0) * cos;
            a2 = SampleType(1.0) - (alpha / a);

            break;

        case filterType::notch:

            b0 = SampleType(1.0);
            b1 = SampleType(-2.0) * cos;
            b2 = SampleType(1.0);
            a0 = SampleType(1.0) + alpha;
            a1 = SampleType(-2.0) * cos;
            a2 = SampleType(1.0) - alpha;

            break;

        default:

            b0 = SampleType(1.0);
            b1 = SampleType(0.0);
            b2 = SampleType(0.0);
            a0 = SampleType(1.0);
            a1 = SampleType(0.0);
            a2 = SampleType(0.0);

            break;

    }

    a0_ = (static_cast <SampleType>(1.0) / a0);
    a1_ = (static_cast <SampleType>((a1 * a0_) * SampleType(-1.0)));
    a2_ = (static_cast <SampleType>((a2 * a0_) * SampleType(-1.0)));
    b0_ = (static_cast <SampleType>(b0 * a0_));
    b1_ = (static_cast <SampleType>(b1 * a0_));
    b2_ = (static_cast <SampleType>(b2 * a0_));
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
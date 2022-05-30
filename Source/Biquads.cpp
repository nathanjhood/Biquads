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

    hz = static_cast<SampleType>(juce::jlimit(minFreq, maxFreq, newFreq));
    frq.setTargetValue(hz);
    coefficients();
}

template <typename SampleType>
void Biquads<SampleType>::setResonance(SampleType newRes)
{
    jassert(static_cast<SampleType>(0.0) <= newRes && newRes <= static_cast<SampleType>(1.0));

    q = static_cast<SampleType>(juce::jlimit(SampleType(0.0), SampleType(1.0), newRes));
    res.setTargetValue(q);
    coefficients();
}

template <typename SampleType>
void Biquads<SampleType>::setGain(SampleType newGain)
{
    g = static_cast<SampleType>(newGain);
    lev.setTargetValue(g);
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
void Biquads<SampleType>::setRampDurationSeconds(double newDurationSeconds) noexcept
{
    if (rampDurationSeconds != newDurationSeconds)
    {
        rampDurationSeconds = newDurationSeconds;
        reset(static_cast<SampleType>(0.0));
    }
}

template <typename SampleType>
double Biquads<SampleType>::getRampDurationSeconds() const noexcept
{
    return rampDurationSeconds;
}

template <typename SampleType>
bool Biquads<SampleType>::isSmoothing() const noexcept
{
    bool compSmoothing = frq.isSmoothing() || res.isSmoothing() || lev.isSmoothing();

    return compSmoothing;
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

    minFreq = static_cast <SampleType>(sampleRate) / static_cast <SampleType>(24576.0);
    maxFreq = static_cast <SampleType>(sampleRate) / static_cast <SampleType>(2.125);

    jassert(static_cast <SampleType>(20.0) >= minFreq && minFreq <= static_cast <SampleType>(20000.0));
    jassert(static_cast <SampleType>(20.0) <= maxFreq && maxFreq >= static_cast <SampleType>(20000.0));

    setFrequency(hz);
    setResonance(q);
    setGain(g);

    coefficients();
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

    frq.reset(sampleRate, rampDurationSeconds);
    res.reset(sampleRate, rampDurationSeconds);
    lev.reset(sampleRate, rampDurationSeconds);

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
    else
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
    SampleType omega = static_cast <SampleType>(frq.getNextValue() * ((pi * two) / sampleRate));
    SampleType cos = static_cast <SampleType>(std::cos(omega));
    SampleType sin = static_cast <SampleType>(std::sin(omega));
    SampleType tan = static_cast <SampleType>(sin / cos);
    SampleType alpha = static_cast <SampleType>(sin * (one - res.getNextValue()));
    SampleType a = static_cast <SampleType>(juce::Decibels::decibelsToGain(static_cast<SampleType>(lev.getNextValue() * static_cast <SampleType>(0.5))));

    juce::ignoreUnused(tan);

    auto sqrtA = (std::sqrt(a) * two) * alpha;

    SampleType b0 = one;
    SampleType b1 = zero;
    SampleType b2 = zero;
    SampleType a0 = one;
    SampleType a1 = zero;
    SampleType a2 = zero;

    switch (filtType)
    {
        case filterType::lowPass2:

            b0 = (one - cos) / two;
            b1 = one - cos;
            b2 = (one - cos) / two;
            a0 = one + alpha;
            a1 = minusTwo * cos;
            a2 = one - alpha;

            break;


        case filterType::lowPass1:

            b0 = omega / (one + omega);
            b1 = omega / (one + omega);
            b2 = zero;
            a0 = one;
            a1 = minusOne * ((one - omega) / (one + omega));
            a2 = zero;

            break;


        case filterType::highPass2:

            b0 = (one + cos) / two;
            b1 = minusOne * (one + cos);
            b2 = (one + cos) / two;
            a0 = one + alpha;
            a1 = minusTwo * cos;
            a2 = one - alpha;

            break;


        case filterType::highPass1:

            b0 = one / (one + omega);
            b1 = (one / (one + omega)) * minusOne;
            b2 = zero;
            a0 = one;
            a1 = ((one - omega) / (one + omega)) * minusOne;
            a2 = zero;

            break;


        case filterType::bandPass:

            b0 = sin / two;
            b1 = zero;
            b2 = minusOne * (sin / two);
            a0 = one + alpha;
            a1 = minusTwo * cos;
            a2 = one - alpha;

            break;


        case filterType::bandPassQ:

            b0 = alpha;
            b1 = zero;
            b2 = minusOne * alpha;
            a0 = one + alpha;
            a1 = minusTwo * cos;
            a2 = one - alpha;

            break;


        case filterType::lowShelf2:

            b0 = (((a + one) - ((a - one) * cos)) + sqrtA) * a;
            b1 = (((a - one) - ((a + one) * cos)) * two) * a;
            b2 = (((a + one) - ((a - one) * cos)) - sqrtA) * a;
            a0 = ((a + one) + ((a - one) * cos)) + sqrtA;
            a1 = ((a - one) + ((a + one) * cos)) * minusTwo;
            a2 = ((a + one) + ((a - one) * cos)) - sqrtA;

            break;


        case filterType::lowShelf1:

            b0 = one + ((omega / (one + omega)) * (minusOne + (a * a)));
            b1 = (((omega / (one + omega)) * (minusOne + (a * a))) - ((one - omega) / (one + omega)));
            b2 = zero;
            a0 = one;
            a1 = minusOne * ((one - omega) / (one + omega));
            a2 = zero;

            break;


        case filterType::lowShelf1C:

            b0 = one + ((omega / a) / (one + (omega / a)) * (minusOne + (a * a)));
            b1 = ((((omega / a) / (one + (omega / a))) * (minusOne + (a * a))) - ((one - (omega / a)) / (one + (omega / a))));
            b2 = zero;
            a0 = one;
            a1 = minusOne * ((one - (omega / a)) / (one + (omega / a)));
            a2 = zero;

            break;


        case filterType::highShelf2:

            b0 = (((a + one) + ((a - one) * cos)) + sqrtA) * a;
            b1 = (((a - one) + ((a + one) * cos)) * minusTwo) * a;
            b2 = (((a + one) + ((a - one) * cos)) - sqrtA) * a;
            a0 = ((a + one) - ((a - one) * cos)) + sqrtA;
            a1 = ((a - one) - ((a + one) * cos)) * two;
            a2 = ((a + one) - ((a - one) * cos)) - sqrtA;

            break;


        case filterType::highShelf1:

            b0 = one + ((minusOne + (a * a)) / (one + omega));
            b1 = minusOne * (((one - omega) / (one + omega)) + ((minusOne + (a * a)) / (one + omega)));
            b2 = zero;
            a0 = one;
            a1 = minusOne * ((one - omega) / (one + omega));
            a2 = zero;

            break;


        case filterType::highShelf1C:

            b0 = one + ((minusOne + (a * a)) / (one + (omega * a)));
            b1 = minusOne * (((one - (omega * a)) / (one + (omega * a))) + ((minusOne + (a * a)) / (one + (omega * a))));
            b2 = zero;
            a0 = one;
            a1 = minusOne * ((one - (omega * a)) / (one + (omega * a)));
            a2 = zero;

            break;
        

        case filterType::peak:

            b0 = one + (alpha * a);
            b1 = minusTwo * cos;
            b2 = one - (alpha * a);
            a0 = one + (alpha / a);
            a1 = minusTwo * cos;
            a2 = one - (alpha / a);

            break;


        case filterType::notch:

            b0 = one;
            b1 = minusTwo * cos;
            b2 = one;
            a0 = one + alpha;
            a1 = minusTwo * cos;
            a2 = one - alpha;

            break;


        case filterType::allPass:

            b0 = one - alpha;
            b1 = minusTwo * cos;
            b2 = one + alpha;
            a0 = one + alpha;
            a1 = minusTwo * cos;
            a2 = one - alpha;

            break;

        
        default:

            b0 = one;
            b1 = zero;
            b2 = zero;
            a0 = one;
            a1 = zero;
            a2 = zero;

            break;
    }

    a0_ = static_cast <SampleType>(one / a0);
    a1_ = static_cast <SampleType>((a1 * a0_) * minusOne);
    a2_ = static_cast <SampleType>((a2 * a0_) * minusOne);
    b0_ = static_cast <SampleType>(b0 * a0_);
    b1_ = static_cast <SampleType>(b1 * a0_);
    b2_ = static_cast <SampleType>(b2 * a0_);
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
/***************************************************************************//**
 * @file stoneydsp_Biquads.hpp
 * @author Nathan J. Hood <nathanjhood@googlemail.com>
 * @brief
 * @version 1.0.0
 * @date 2024-02-21
 *
 * @copyright Copyright (c) 2024
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 ******************************************************************************/

namespace StoneyDSP {
/** @addtogroup StoneyDSP @{ */

namespace Audio {
/** @addtogroup StoneyDSP::Audio @{ */

template <typename SampleType>
Biquads<SampleType>::Biquads()
: b0            (StoneyDSP::Maths::Constants<SampleType>::one)
, b1            (StoneyDSP::Maths::Constants<SampleType>::zero)
, b2            (StoneyDSP::Maths::Constants<SampleType>::zero)
, a0            (StoneyDSP::Maths::Constants<SampleType>::one)
, a1            (StoneyDSP::Maths::Constants<SampleType>::zero)
, a2            (StoneyDSP::Maths::Constants<SampleType>::zero)
, b_0           (StoneyDSP::Maths::Constants<SampleType>::one)
, b_1           (StoneyDSP::Maths::Constants<SampleType>::zero)
, b_2           (StoneyDSP::Maths::Constants<SampleType>::zero)
, a_0           (StoneyDSP::Maths::Constants<SampleType>::one)
, a_1           (StoneyDSP::Maths::Constants<SampleType>::zero)
, a_2           (StoneyDSP::Maths::Constants<SampleType>::zero)
, loop          (StoneyDSP::Maths::Constants<SampleType>::zero)
, outputSample  (StoneyDSP::Maths::Constants<SampleType>::zero)
, minFrequency  (static_cast <SampleType>(20.0))
, maxFrequency  (static_cast <SampleType>(20000.0))
, hz            (static_cast <SampleType>(1000.0))
, q             (static_cast <SampleType>(0.5))
, g             (StoneyDSP::Maths::Constants<SampleType>::zero)
{
    reset(zero);
    update();
}


template <typename SampleType>
void Biquads<SampleType>::setFrequency (SampleType newFrequency)
{
    jassert(minFrequency <= newFrequency && newFrequency <= maxFrequency);

    hz = juce::jlimit(minFrequency, maxFrequency, newFrequency);

    omega = (hz * ((pi * two) / static_cast <SampleType>(sampleRate)));
    cos = (std::cos(omega));
    sin = (std::sin(omega));

    update(); // calculateCoefficients();
}

template <typename SampleType>
void Biquads<SampleType>::setResonance(SampleType newResonance)
{
    jassert(zero <= newResonance && newResonance <= one);

    q = juce::jlimit(zero, one, newResonance);
    update(); // calculateCoefficients();
}

template <typename SampleType>
void Biquads<SampleType>::setGain(SampleType newGain)
{
    g = newGain;
    update(); // calculateCoefficients();
}

template <typename SampleType>
void Biquads<SampleType>::setFilterType(filterType newFilterType)
{
    if (filterTypeParamValue != newFilterType)
    {
        filterTypeParamValue = newFilterType;

        reset(zero);
        update(); // calculateCoefficients();
    }
}

template <typename SampleType>
void Biquads<SampleType>::setTransformType(transformationType newTransformationType)
{
    if (transformationParamValue != newTransformationType)
    {
        transformationParamValue = newTransformationType;
        reset(zero);
        update(); // calculateCoefficients();
    }
}

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

    minFrequency = static_cast <SampleType> (sampleRate / 24576.0);
    maxFrequency = static_cast <SampleType> (sampleRate / 2.125);

    jassert(static_cast <SampleType> (20.0) >= minFrequency && minFrequency <= static_cast <SampleType> (20000.0));
    jassert(static_cast <SampleType> (20.0) <= maxFrequency && maxFrequency >= static_cast <SampleType> (20000.0));

    reset(zero);

    setFrequency(hz);
    setResonance(q);
    setGain(g);
    setFilterType(filterTypeParamValue);
    setTransformType(transformationParamValue);

    update(); // calculateCoefficients();
}

template <typename SampleType>
void Biquads<SampleType>::reset()
{
    SampleType initialValue = zero;

    for (auto v : { &Wn_1, &Wn_2, &Xn_1, &Xn_2, &Yn_1, &Yn_2 })
        std::fill(v->begin(), v->end(), initialValue);
}

template <typename SampleType>
void Biquads<SampleType>::reset(SampleType initialValue)
{
    for (auto v : { &Wn_1, &Wn_2, &Xn_1, &Xn_2, &Yn_1, &Yn_2 })
        std::fill(v->begin(), v->end(), initialValue);
}

template <typename SampleType>
SampleType Biquads<SampleType>::processSample(int channel, SampleType inputValue)
{
    jassert(juce::isPositiveAndBelow(channel, Wn_1.size()));
    jassert(juce::isPositiveAndBelow(channel, Wn_2.size()));
    jassert(juce::isPositiveAndBelow(channel, Xn_1.size()));
    jassert(juce::isPositiveAndBelow(channel, Xn_2.size()));
    jassert(juce::isPositiveAndBelow(channel, Yn_1.size()));
    jassert(juce::isPositiveAndBelow(channel, Yn_1.size()));

    switch (transformationParamValue)
    {
    case StoneyDSP::Audio::BiquadsBiLinearTransformationType::directFormI:
        inputValue = directFormI(channel, inputValue);
        break;
    case StoneyDSP::Audio::BiquadsBiLinearTransformationType::directFormII:
        inputValue = directFormII(channel, inputValue);
        break;
    case StoneyDSP::Audio::BiquadsBiLinearTransformationType::directFormItransposed:
        inputValue = directFormITransposed(channel, inputValue);
        break;
    case StoneyDSP::Audio::BiquadsBiLinearTransformationType::directFormIItransposed:
        inputValue = directFormIITransposed(channel, inputValue);
        break;
    default:
        inputValue = directFormIITransposed(channel, inputValue);
    }

    return inputValue;
}

template <typename SampleType>
SampleType Biquads<SampleType>::directFormI(int channel, SampleType inputSample)
{
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];
    auto& Yn1 = Yn_1[(size_t)channel];
    auto& Yn2 = Yn_2[(size_t)channel];

    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Yn = ((Xn * b0) + (Xn1 * b1) + (Xn2 * b2) + (Yn1 * a1) + (Yn2 * a2));

    Xn2 = Xn1, Yn2 = Yn1;
    Xn1 = Xn, Yn1 = Yn;

    return Yn;
}

template <typename SampleType>
SampleType Biquads<SampleType>::directFormII(int channel, SampleType inputSample)
{
    auto& Wn1 = Wn_1[(size_t)channel];
    auto& Wn2 = Wn_2[(size_t)channel];

    auto& Wn = loop;
    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Wn = ( Xn + ((Wn1 * a1) + (Wn2 * a2)));
    Yn = ((Wn * b0) + (Wn1 * b1) + (Wn2 * b2));

    Wn2 = Wn1;
    Wn1 = Wn;

    return Yn;
}

template <typename SampleType>
SampleType Biquads<SampleType>::directFormITransposed(int channel, SampleType inputSample)
{
    auto& Wn1 = Wn_1[(size_t)channel];
    auto& Wn2 = Wn_2[(size_t)channel];
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];

    auto& Wn = loop;
    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Wn = ( Xn + Wn2);
    Yn = ((Wn * b0) + Xn2);

    Xn2 = ((Wn * b1) + Xn1), Wn2 = ((Wn * a1) + Wn1);
    Xn1 = ( Wn * b2),        Wn1 = ( Wn * a2);

    return Yn;
}

template <typename SampleType>
SampleType Biquads<SampleType>::directFormIITransposed(int channel, SampleType inputSample)
{
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];

    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Yn  = ((Xn * b0) + (Xn2));

    Xn2 = ((Xn * b1) + (Xn1) + (Yn * a1));
    Xn1 = ((Xn * b2) +         (Yn * a2));

    return Yn;
}

template <typename SampleType>
void Biquads<SampleType>::calculateCoefficients()
{
    alpha = (sin * (one - q));
    a = (std::pow(SampleType(10), (g * SampleType(0.05))));
    sqrtA = ((std::sqrt(a) * two) * alpha);

    switch (filterTypeParamValue)
    {
    case filterType::lowPass2:

        b_0 = ((one - cos) / two);
        b_1 = (one - cos);
        b_2 = ((one - cos) / two);
        a_0 = (one + alpha);
        a_1 = (minusTwo * cos);
        a_2 = (one - alpha);

        break;


    case filterType::lowPass1:

        b_0 = (omega / (one + omega));
        b_1 = (omega / (one + omega));
        b_2 = (zero);
        a_0 = (one);
        a_1 = (minusOne * ((one - omega) / (one + omega)));
        a_2 = (zero);

        break;


    case filterType::highPass2:

        b_0 = (one + cos) / two;
        b_1 = (minusOne * (one + cos));
        b_2 = ((one + cos) / two);
        a_0 = (one + alpha);
        a_1 = (minusTwo * cos);
        a_2 = (one - alpha);

        break;


    case filterType::highPass1:

        b_0 = (one / (one + omega));
        b_1 = ((one / (one + omega)) * minusOne);
        b_2 = (zero);
        a_0 = (one);
        a_1 = (((one - omega) / (one + omega)) * minusOne);
        a_2 = (zero);

        break;


    case filterType::bandPass:

        b_0 = (sin / two);
        b_1 = (zero);
        b_2 = (minusOne * (sin / two));
        a_0 = (one + alpha);
        a_1 = (minusTwo * cos);
        a_2 = (one - alpha);

        break;


    case filterType::bandPassQ:

        b_0 = (alpha);
        b_1 = (zero);
        b_2 = (minusOne * alpha);
        a_0 = (one + alpha);
        a_1 = (minusTwo * cos);
        a_2 = (one - alpha);

        break;


    case filterType::lowShelf2:

        b_0 = ((((a + one) - ((a - one) * cos)) + sqrtA) * a);
        b_1 = ((((a - one) - ((a + one) * cos)) * two) * a);
        b_2 = ((((a + one) - ((a - one) * cos)) - sqrtA) * a);
        a_0 = (((a + one) + ((a - one) * cos)) + sqrtA);
        a_1 = (((a - one) + ((a + one) * cos)) * minusTwo);
        a_2 = (((a + one) + ((a - one) * cos)) - sqrtA);

        break;


    case filterType::lowShelf1:

        b_0 = (one + ((omega / (one + omega)) * (minusOne + (a * a))));
        b_1 = ((((omega / (one + omega)) * (minusOne + (a * a))) - ((one - omega) / (one + omega))));
        b_2 = (zero);
        a_0 = (one);
        a_1 = (minusOne * ((one - omega) / (one + omega)));
        a_2 = (zero);

        break;


    case filterType::lowShelf1C:

        b_0 = (one + ((omega / a) / (one + (omega / a)) * (minusOne + (a * a))));
        b_1 = (((((omega / a) / (one + (omega / a))) * (minusOne + (a * a))) - ((one - (omega / a)) / (one + (omega / a)))));
        b_2 = (zero);
        a_0 = (one);
        a_1 = (minusOne * ((one - (omega / a)) / (one + (omega / a))));
        a_2 = (zero);

        break;


    case filterType::highShelf2:

        b_0 = ((((a + one) + ((a - one) * cos)) + sqrtA) * a);
        b_1 = ((((a - one) + ((a + one) * cos)) * minusTwo) * a);
        b_2 = ((((a + one) + ((a - one) * cos)) - sqrtA) * a);
        a_0 = (((a + one) - ((a - one) * cos)) + sqrtA);
        a_1 = (((a - one) - ((a + one) * cos)) * two);
        a_2 = (((a + one) - ((a - one) * cos)) - sqrtA);

        break;


    case filterType::highShelf1:

        b_0 = (one + ((minusOne + (a * a)) / (one + omega)));
        b_1 = (minusOne * (((one - omega) / (one + omega)) + ((minusOne + (a * a)) / (one + omega))));
        b_2 = (zero);
        a_0 = (one);
        a_1 = (minusOne * ((one - omega) / (one + omega)));
        a_2 = (zero);

        break;


    case filterType::highShelf1C:

        b_0 = (one + ((minusOne + (a * a)) / (one + (omega * a))));
        b_1 = (minusOne * (((one - (omega * a)) / (one + (omega * a))) + ((minusOne + (a * a)) / (one + (omega * a)))));
        b_2 = (zero);
        a_0 = (one);
        a_1 = (minusOne * ((one - (omega * a)) / (one + (omega * a))));
        a_2 = (zero);

        break;


    case filterType::peak:

        b_0 = (one + (alpha * a));
        b_1 = (minusTwo * cos);
        b_2 = (one - (alpha * a));
        a_0 = (one + (alpha / a));
        a_1 = (minusTwo * cos);
        a_2 = (one - (alpha / a));

        break;


    case filterType::notch:

        b_0 = (one);
        b_1 = (minusTwo * cos);
        b_2 = (one);
        a_0 = (one + alpha);
        a_1 = (minusTwo * cos);
        a_2 = (one - alpha);

        break;


    case filterType::allPass:

        b_0 = (one - alpha);
        b_1 = (minusTwo * cos);
        b_2 = (one + alpha);
        a_0 = (one + alpha);
        a_1 = (minusTwo * cos);
        a_2 = (one - alpha);

        break;


    default:

        b_0 = one;
        b_1 = zero;
        b_2 = zero;
        a_0 = one;
        a_1 = zero;
        a_2 = zero;

        break;
    }

    a0 = (  one  / a_0);
    a1 = ((-a_1) * a0);
    a2 = ((-a_2) * a0);
    b0 = (  b_0  * a0);
    b1 = (  b_1  * a0);
    b2 = (  b_2  * a0);
}

template <typename SampleType>
void Biquads<SampleType>::snapToZero() noexcept
{
    for (auto v : { &Wn_1, &Wn_2, &Xn_1, &Xn_2, &Yn_1, &Yn_2 })
        for (auto& element : *v)
            juce::dsp::util::snapToZero(element);
}

template <typename SampleType>
void Biquads<SampleType>::update()
{
    calculateCoefficients();
}

//==============================================================================
template class Biquads<float>;
template class Biquads<double>;

  /// @} group StoneyDSP::Audio
} // namespace Audio

  /// @} group StoneyDSP
} // namespace StoneyDSP

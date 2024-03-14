/**
 * @file stoneydsp_Biquads.cpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief
 * @version 0.1
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */

namespace StoneyDSP
{
/** @addtogroup StoneyDSP
 *  @{
 */

namespace Audio
{
/** @addtogroup Audio
 *  @{
 */

/**
 * @brief Construct a new 'Biquads' object.
 *
 * @tparam SampleType
 */
template <typename SampleType>
Biquads<SampleType>::Biquads()
 : b0           (static_cast <SampleType>(1.0))
 , b1           (static_cast <SampleType>(0.0))
 , b2           (static_cast <SampleType>(0.0))
 , a0           (static_cast <SampleType>(1.0))
 , a1           (static_cast <SampleType>(0.0))
 , a2           (static_cast <SampleType>(0.0))
 , b_0          (static_cast <SampleType>(1.0))
 , b_1          (static_cast <SampleType>(0.0))
 , b_2          (static_cast <SampleType>(0.0))
 , a_0          (static_cast <SampleType>(1.0))
 , a_1          (static_cast <SampleType>(0.0))
 , a_2          (static_cast <SampleType>(0.0))
 , loop         (static_cast <SampleType>(0.0))
 , outputSample (static_cast <SampleType>(0.0))
 , minFreq      (static_cast <SampleType>(20.0))
 , maxFreq      (static_cast <SampleType>(20000.0))
 , hz           (static_cast <SampleType>(1000.0))
 , q            (static_cast <SampleType>(0.5))
 , g            (static_cast <SampleType>(0.0))
{
    reset();
}

/**
 * @brief Sets the centre Frequency of the filter.
 *
 * @tparam SampleType
 * @param newFreq Frequency in Hz. Range = 20..20000.
 */
template <typename SampleType>
void Biquads<SampleType>::setFrequency (SampleType newFreq)
{
    jassert(minFreq <= newFreq && newFreq <= maxFreq);

    // if (hz != newFreq)
    // {
        hz = juce::jlimit(minFreq, maxFreq, newFreq);

        omega = (hz * ((pi * two) / static_cast <SampleType>(sampleRate)));
        cos = (std::cos(omega));
        sin = (std::sin(omega));

        calculateCoefficients();
    // }
}

/**
 * @brief Sets the resonance of the filter.
 *
 * @tparam SampleType
 * @param newRes Range = 0..1.
 */
template <typename SampleType>
void Biquads<SampleType>::setResonance(SampleType newRes)
{
    jassert(zero <= newRes && newRes <= one);

    // if (q != newRes)
    // {
        q = juce::jlimit(SampleType(0.0), SampleType(1.0), newRes);

        calculateCoefficients();
    // }
}

/**
 * @brief Sets the centre Frequency gain of the filter
 * (Peak and shelf modes only).
 *
 * @tparam SampleType
 * @param newGain Filter gain in dB.
 */
template <typename SampleType>
void Biquads<SampleType>::setGain(SampleType newGain)
{
    // if (g != newGain)
    // {
        g = newGain;

        calculateCoefficients();
    // }
}

/**
 * @brief Sets the type of the filter.
 *
 * @tparam SampleType
 * @param newFiltType See 'FilterType' enum for available types.
 */
template <typename SampleType>
void Biquads<SampleType>::setFilterType(filterType newFiltType)
{
    if (filtType != newFiltType)
    {
        filtType = newFiltType;

        reset();
        calculateCoefficients();
    }
}

/**
 * @brief Sets the BiLinear Transform for the filter to use.
 *
 * @tparam SampleType
 * @param newTransformType See enum for available types.
 */
template <typename SampleType>
void Biquads<SampleType>::setTransformType(transformationType newTransformType)
{
    if (transformType != newTransformType)
    {
        transformType = newTransformType;
        reset();
    }
}

/**
 * @brief Initialises the processor.
 *
 * @tparam SampleType
 * @param spec
 */
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

    minFreq = static_cast <SampleType> (sampleRate / 24576.0);
    maxFreq = static_cast <SampleType> (sampleRate / 2.125);

    jassert(static_cast <SampleType> (20.0) >= minFreq && minFreq <= static_cast <SampleType> (20000.0));
    jassert(static_cast <SampleType> (20.0) <= maxFreq && maxFreq >= static_cast <SampleType> (20000.0));

    reset();

    setFrequency(hz);
    setResonance(q);
    setGain(g);
    setFilterType(filtType);
    setTransformType(transformType);
}

/**
 * @brief Resets the internal state variables of the processor.
 *
 * @tparam SampleType
 * @param initialValue
 */
template <typename SampleType>
void Biquads<SampleType>::reset(SampleType initialValue)
{
    for (auto v : { &Wn_1, &Wn_2, &Xn_1, &Xn_2, &Yn_1, &Yn_2 })
        std::fill(v->begin(), v->end(), initialValue);
}

/**
 * @brief Processes one sample at a time on a given channel.
 *
 * @tparam SampleType
 * @param channel
 * @param inputValue
 * @return SampleType
 */
template <typename SampleType>
SampleType Biquads<SampleType>::processSample(int channel, SampleType inputValue)
{
    jassert(juce::isPositiveAndBelow(channel, Wn_1.size()));
    jassert(juce::isPositiveAndBelow(channel, Wn_2.size()));
    jassert(juce::isPositiveAndBelow(channel, Xn_1.size()));
    jassert(juce::isPositiveAndBelow(channel, Xn_2.size()));
    jassert(juce::isPositiveAndBelow(channel, Yn_1.size()));
    jassert(juce::isPositiveAndBelow(channel, Yn_1.size()));

    switch (transformType)
    {
    case TransformationType::directFormI:
        inputValue = directFormI(channel, inputValue);
        break;
    case TransformationType::directFormII:
        inputValue = directFormII(channel, inputValue);
        break;
    case TransformationType::directFormItransposed:
        inputValue = directFormITransposed(channel, inputValue);
        break;
    case TransformationType::directFormIItransposed:
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

    Wn = (Xn + ((Wn1 * a1) + (Wn2 * a2)));
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

    Wn = (Xn + Wn2);
    Yn = ((Wn * b0) + Xn2);

    Xn2 = ((Wn * b1) + Xn1), Wn2 = ((Wn * a1) + Wn1);
    Xn1 = (Wn * b2), Wn1 = (Wn * a2);

    return Yn;
}

template <typename SampleType>
SampleType Biquads<SampleType>::directFormIITransposed(int channel, SampleType inputSample)
{
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];

    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Yn = ((Xn * b0) + (Xn2));

    Xn2 = ((Xn * b1) + (Xn1) + (Yn * a1));
    Xn1 = ((Xn * b2) + (Yn * a2));

    return Yn;
}

template <typename SampleType>
void Biquads<SampleType>::calculateCoefficients()
{
    alpha = (sin * (one - q));
    a = (std::pow(SampleType(10), (g * SampleType(0.05))));
    sqrtA = ((std::sqrt(a) * two) * alpha);

    switch (filtType)
    {
    case filterType::lowPass2:

        b_0 = (one - cos) / two;
        b_1 = one - cos;
        b_2 = (one - cos) / two;
        a_0 = one + alpha;
        a_1 = minusTwo * cos;
        a_2 = one - alpha;

        break;


    case filterType::lowPass1:

        b_0 = omega / (one + omega);
        b_1 = omega / (one + omega);
        b_2 = zero;
        a_0 = one;
        a_1 = minusOne * ((one - omega) / (one + omega));
        a_2 = zero;

        break;


    case filterType::highPass2:

        b_0 = (one + cos) / two;
        b_1 = minusOne * (one + cos);
        b_2 = (one + cos) / two;
        a_0 = one + alpha;
        a_1 = minusTwo * cos;
        a_2 = one - alpha;

        break;


    case filterType::highPass1:

        b_0 = one / (one + omega);
        b_1 = (one / (one + omega)) * minusOne;
        b_2 = zero;
        a_0 = one;
        a_1 = ((one - omega) / (one + omega)) * minusOne;
        a_2 = zero;

        break;


    case filterType::bandPass:

        b_0 = sin / two;
        b_1 = zero;
        b_2 = minusOne * (sin / two);
        a_0 = one + alpha;
        a_1 = minusTwo * cos;
        a_2 = one - alpha;

        break;


    case filterType::bandPassQ:

        b_0 = alpha;
        b_1 = zero;
        b_2 = minusOne * alpha;
        a_0 = one + alpha;
        a_1 = minusTwo * cos;
        a_2 = one - alpha;

        break;


    case filterType::lowShelf2:

        b_0 = (((a + one) - ((a - one) * cos)) + sqrtA) * a;
        b_1 = (((a - one) - ((a + one) * cos)) * two) * a;
        b_2 = (((a + one) - ((a - one) * cos)) - sqrtA) * a;
        a_0 = ((a + one) + ((a - one) * cos)) + sqrtA;
        a_1 = ((a - one) + ((a + one) * cos)) * minusTwo;
        a_2 = ((a + one) + ((a - one) * cos)) - sqrtA;

        break;


    case filterType::lowShelf1:

        b_0 = one + ((omega / (one + omega)) * (minusOne + (a * a)));
        b_1 = (((omega / (one + omega)) * (minusOne + (a * a))) - ((one - omega) / (one + omega)));
        b_2 = zero;
        a_0 = one;
        a_1 = minusOne * ((one - omega) / (one + omega));
        a_2 = zero;

        break;


    case filterType::lowShelf1C:

        b_0 = one + ((omega / a) / (one + (omega / a)) * (minusOne + (a * a)));
        b_1 = ((((omega / a) / (one + (omega / a))) * (minusOne + (a * a))) - ((one - (omega / a)) / (one + (omega / a))));
        b_2 = zero;
        a_0 = one;
        a_1 = minusOne * ((one - (omega / a)) / (one + (omega / a)));
        a_2 = zero;

        break;


    case filterType::highShelf2:

        b_0 = (((a + one) + ((a - one) * cos)) + sqrtA) * a;
        b_1 = (((a - one) + ((a + one) * cos)) * minusTwo) * a;
        b_2 = (((a + one) + ((a - one) * cos)) - sqrtA) * a;
        a_0 = ((a + one) - ((a - one) * cos)) + sqrtA;
        a_1 = ((a - one) - ((a + one) * cos)) * two;
        a_2 = ((a + one) - ((a - one) * cos)) - sqrtA;

        break;


    case filterType::highShelf1:

        b_0 = one + ((minusOne + (a * a)) / (one + omega));
        b_1 = minusOne * (((one - omega) / (one + omega)) + ((minusOne + (a * a)) / (one + omega)));
        b_2 = zero;
        a_0 = one;
        a_1 = minusOne * ((one - omega) / (one + omega));
        a_2 = zero;

        break;


    case filterType::highShelf1C:

        b_0 = one + ((minusOne + (a * a)) / (one + (omega * a)));
        b_1 = minusOne * (((one - (omega * a)) / (one + (omega * a))) + ((minusOne + (a * a)) / (one + (omega * a))));
        b_2 = zero;
        a_0 = one;
        a_1 = minusOne * ((one - (omega * a)) / (one + (omega * a)));
        a_2 = zero;

        break;


    case filterType::peak:

        b_0 = one + (alpha * a);
        b_1 = minusTwo * cos;
        b_2 = one - (alpha * a);
        a_0 = one + (alpha / a);
        a_1 = minusTwo * cos;
        a_2 = one - (alpha / a);

        break;


    case filterType::notch:

        b_0 = one;
        b_1 = minusTwo * cos;
        b_2 = one;
        a_0 = one + alpha;
        a_1 = minusTwo * cos;
        a_2 = one - alpha;

        break;


    case filterType::allPass:

        b_0 = one - alpha;
        b_1 = minusTwo * cos;
        b_2 = one + alpha;
        a_0 = one + alpha;
        a_1 = minusTwo * cos;
        a_2 = one - alpha;

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

    a0 = (one / a_0);
    a1 = ((-a_1) * a0);
    a2 = ((-a_2) * a0);
    b0 = (b_0 * a0);
    b1 = (b_1 * a0);
    b2 = (b_2 * a0);
}

/**
 * @brief Ensure that the state variables are rounded to zero if the state
 * variables are denormals. This is only needed if you are doing sample
 *  by sample processing.
 *
 * @tparam SampleType
 */
template <typename SampleType>
void Biquads<SampleType>::snapToZero() noexcept
{
    for (auto v : { &Wn_1, &Wn_2, &Xn_1, &Xn_2, &Yn_1, &Yn_2 })
        for (auto& element : *v)
            juce::dsp::util::snapToZero(element);
}

//==============================================================================
template class Biquads<float>;
template class Biquads<double>;

  /// @} group Audio
} // namespace Audio

  /// @} group StoneyDSP
} // namespace StoneyDSP

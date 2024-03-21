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

/** @brief A list of the types of the filter. */
enum struct BiquadsFilterType
{
    lowPass2 = 0,
    lowPass1 = 1,
    highPass2 = 2,
    highPass1 = 3,
    bandPass = 4,
    bandPassQ = 5,
    lowShelf2 = 6,
    lowShelf1 = 7,
    lowShelf1C = 8,
    highShelf2 = 9,
    highShelf1 = 10,
    highShelf1C = 11,
    peak = 12,
    notch = 13,
    allPass = 14
};

enum struct BiquadsBiLinearTransformationType
{
    directFormI = 0,
    directFormII = 1,
    directFormItransposed = 2,
    directFormIItransposed = 3
};

/**
 * @brief The 'Biquads' class.
 *
 * @tparam SampleType
 */
template <typename SampleType>
class Biquads
{
public:
    using filterType            = StoneyDSP::Audio::BiquadsFilterType;
    using transformationType    = StoneyDSP::Audio::BiquadsBiLinearTransformationType;
    //==============================================================================
    /** Constructor. */
    Biquads();

    //==============================================================================
    /**
     * @brief Sets the cutoff frequency of the filter.
     * @param newFrequencyHz the new cutoff frequency in Hz.
     */
    void setFrequency(SampleType newFrequencyHz);
    /**
     * @brief Sets the resonance of the filter.
     * @param newResonance the new resonance amount from 0 (min) to 1 (max).
     */
    void setResonance(SampleType newResonance);
    /**
     * @brief Sets the gain of the filter.
     * @param newGain the new gain amount in Decibels.
     */
    void setGain(SampleType newGain);
    /**
     * @brief Sets the type of the filter.
     * @param newFilterType the new filter type.
     */
    void setFilterType(filterType newFilterType);
    /**
     * @brief Sets the BiLinear Transform type of the filter.
     * @param newTransformType the new transformation type.
     */
    void setTransformType(transformationType newTransformType);

    //==============================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();
    /**
     * @brief Resets the internal state variables of the processor to the
     * passed-in value.
     *
     * @param initialValue
     */
    void reset(SampleType initialValue = { 0.0 });
    /**
     * @brief Ensure that the state variables are rounded to zero if the state
     * variables are denormals. This is only needed if you are doing sample by
     * sample processing.
     *
     */
    void snapToZero() noexcept;

    //==============================================================================
    /** Processes the input and output samples supplied in the processing context. */
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock      = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples  = outputBlock.getNumSamples();

        jassert (inputBlock.getNumChannels() == numChannels);
        jassert (inputBlock.getNumSamples()  == numSamples);

        if (context.isBypassed)
        {
            outputBlock.copyFrom (inputBlock);
            return;
        }

        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            auto* inputSamples  = inputBlock .getChannelPointer (channel);
            auto* outputSamples = outputBlock.getChannelPointer (channel);

            for (size_t i = 0; i < numSamples; ++i)
                outputSamples[i] = processSample ((int) channel, inputSamples[i]);
        }
    }

    /** Performs the processing operation on a single sample at a time. */
    SampleType processSample (int channel, SampleType inputValue);

private:
    //==============================================================================
    /** Updates the internal state variables of the processor. */
    void update();

    void calculateCoefficients();

    SampleType directFormI              (int channel, SampleType inputValue);
    SampleType directFormII             (int channel, SampleType inputValue);
    SampleType directFormITransposed    (int channel, SampleType inputValue);
    SampleType directFormIITransposed   (int channel, SampleType inputValue);

    //==============================================================================
    /** Unit-delay object(s). */
    std::vector<SampleType> Wn_1, Wn_2, Xn_1, Xn_2, Yn_1, Yn_2;

    /** Coefficient gain(s). */
    StoneyDSP::Maths::Coefficient<SampleType> b0, b1, b2, a0, a1, a2;

    /** Coefficient calculation(s). */
    StoneyDSP::Maths::Coefficient<SampleType> b_0, b_1, b_2, a_0, a_1, a_2;

    filterType filterTypeParamValue = { filterType::peak };
    transformationType transformationParamValue = { transformationType::directFormIItransposed };

    //==============================================================================
    /** Initialised parameter(s) */
    SampleType
        loop = static_cast<SampleType>(0.0)
        , outputSample = static_cast<SampleType>(0.0)
        , minFrequency = static_cast<SampleType>(20.0)
        , maxFrequency = static_cast<SampleType>(20000.0)
        , hz = static_cast<SampleType>(1000.0)
        , q = static_cast<SampleType>(0.5)
        , g = static_cast<SampleType>(0.0)
    ;

    SampleType omega, cos, sin, tan, alpha, a, sqrtA { static_cast<SampleType>(0.0) };

    //==============================================================================
    /** Initialised constant */
    const SampleType zero       = StoneyDSP::Maths::Constants<SampleType>::zero;
    const SampleType one        = StoneyDSP::Maths::Constants<SampleType>::one;
    const SampleType two        = StoneyDSP::Maths::Constants<SampleType>::two;
    const SampleType minusOne   = StoneyDSP::Maths::Constants<SampleType>::minusOne;
    const SampleType minusTwo   = StoneyDSP::Maths::Constants<SampleType>::minusTwo;

    const SampleType pi         = juce::MathConstants<SampleType>::pi;
    double sampleRate = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Biquads)
};

  /// @} group StoneyDSP::Audio
} // namespace Audio

  /// @} group StoneyDSP
} // namespace StoneyDSP

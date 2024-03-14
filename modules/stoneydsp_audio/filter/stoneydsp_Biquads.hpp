/**
 * @file stoneydsp_Biquads.hpp
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
 * @brief The 'Biquads' class.
 *
 * @tparam SampleType
 */
template <typename SampleType>
class Biquads
{
public:

    using filterType = FilterType;
    using transformationType = TransformationType;

    Biquads();

    void setFrequency(SampleType newFreq);
    void setResonance(SampleType newRes);
    void setGain(SampleType newGain);
    void setFilterType(filterType newFiltType);
    void setTransformType(transformationType newTransformType);

    void prepare(juce::dsp::ProcessSpec& spec);
    void reset(SampleType initialValue = { 0.0 });
    void snapToZero() noexcept;

    /**
     * @brief  Processes the input and output samples supplied in the
     * processing context.
     *
     * @tparam ProcessContext
     * @param context
     */
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();
        //const auto len = inputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

        if (context.isBypassed)
        {
            outputBlock.copyFrom(inputBlock);
            return;
        }

        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            auto* inputSamples = inputBlock.getChannelPointer(channel);
            auto* outputSamples = outputBlock.getChannelPointer(channel);

            for (size_t i = 0; i < numSamples; ++i)
                outputSamples[i] = processSample((int)channel, inputSamples[i]);
        }

#if JUCE_DSP_ENABLE_SNAP_TO_ZERO
        snapToZero();
#endif
    }

    SampleType processSample(int channel, SampleType inputValue);

    //==========================================================================
    /**
     * @brief Coefficient current value. Safe to pass i.e. to the display thread.
     *
     * @return SampleType
     */
    SampleType geta0() { return a0.get(); }
    /**
     * @brief Coefficient current value. Safe to pass i.e. to the display thread.
     *
     * @return SampleType
     */
    SampleType getb0() { return b0.get(); }
    /**
     * @brief Coefficient current value. Safe to pass i.e. to the display thread.
     *
     * @return SampleType
     */
    SampleType geta1() { return a1.get(); }
    /**
     * @brief Coefficient current value. Safe to pass i.e. to the display thread.
     *
     * @return SampleType
     */
    SampleType getb1() { return b1.get(); }
    /**
     * @brief Coefficient current value. Safe to pass i.e. to the display thread.
     *
     * @return SampleType
     */
    SampleType geta2() { return a2.get(); }
    /**
     * @brief Coefficient current value. Safe to pass i.e. to the display thread.
     *
     * @return SampleType
     */
    SampleType getb2() { return b2.get(); }

private:

    void calculateCoefficients();

    SampleType directFormI(int channel, SampleType inputValue);
    SampleType directFormII(int channel, SampleType inputValue);
    SampleType directFormITransposed(int channel, SampleType inputValue);
    SampleType directFormIITransposed(int channel, SampleType inputValue);

    /**
     * @brief Unit-delay object(s).
     *
     */
    std::vector<SampleType> Wn_1, Wn_2, Xn_1, Xn_2, Yn_1, Yn_2;

    /**
     * @brief Coefficient gain(s).
     *
     */
    StoneyDSP::Maths::Coefficient<SampleType> b0, b1, b2, a0, a1, a2;

    /** Coefficient calculation(s). */
    StoneyDSP::Maths::Coefficient<SampleType> b_0, b_1, b_2, a_0, a_1, a_2;

    /** Initialised parameter(s) */
    SampleType
        loop = 0.0
        , outputSample = 0.0
        , minFreq = 20.0
        , maxFreq = 20000.0
        , hz = 1000.0
        , q = 0.5
        , g = 0.0
    ;

    filterType filtType = filterType::lowPass2;
    transformationType transformType = transformationType::directFormIItransposed;

    SampleType omega, cos, sin, tan, alpha, a, sqrtA { 0.0 };

    //==========================================================================
    /** Initialised constant */
    const SampleType zero = 0.0, one = 1.0, two = 2.0, minusOne = -1.0, minusTwo = -2.0;
    const SampleType pi = juce::MathConstants<SampleType>::pi;
    double sampleRate = 48000.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Biquads)
};

  /// @} group Audio
} // namespace Audio

  /// @} group StoneyDSP
} // namespace StoneyDSP

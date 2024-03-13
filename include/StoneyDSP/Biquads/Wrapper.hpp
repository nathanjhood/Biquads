/**
 * @file Wrapper.hpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief
 * @version 1.2.0
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#define STONEYDSP_BIQUADS_WRAPPER_HPP_INCLUDED

// #include <JuceHeader.h>

#include "StoneyDSP/Biquads.hpp"

namespace StoneyDSP
{
/** @addtogroup StoneyDSP
 *  @{
 */

/**
 * @brief The ```StoneyDSP::Biquads``` namespace.
 *
 */
namespace Biquads
{
/** @addtogroup Biquads
 *  @{
 */

class AudioPluginAudioProcessor;

class AudioPluginAudioProcessorParameters;

template <typename SampleType>
class AudioPluginAudioProcessorWrapper
{
public:
    using filterType            = StoneyDSP::Audio::FilterType;
    using transformationType    = StoneyDSP::Audio::TransformationType;
    //==============================================================================
    /** Constructor. */
    AudioPluginAudioProcessorWrapper(AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts, juce::dsp::ProcessSpec& spec);

    //==============================================================================
    void setFrequency(SampleType newFreq);
    void setResonance(SampleType newRes);
    void setGain(SampleType newGain);
    void setFilterType(filterType newFiltType);
    void setTransformType(transformationType newTransformType);

    //==============================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();
    void reset(SampleType initialValue = { 0.0 });
    void snapToZero() noexcept;

    //==============================================================================
    void process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages);
    void processBlock(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages);
    void processBypass(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages);

    /**
     * @brief  Processes the input and output samples supplied in the
     * processing context.
     *
     * @tparam ProcessContext
     * @param context
     */
    template <typename ProcessContext>
    void processContext(const ProcessContext& context) noexcept
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

    //==============================================================================
    /** Updates the internal state variables of the processor. */
    void update();

private:
    void calculateCoefficients();

    SampleType directFormI              (int channel, SampleType inputValue);
    SampleType directFormII             (int channel, SampleType inputValue);
    SampleType directFormITransposed    (int channel, SampleType inputValue);
    SampleType directFormIITransposed   (int channel, SampleType inputValue);

    //==============================================================================
    // This reference is provided as a quick way for the wrapper to
    // access the processor object that created it.

    AudioPluginAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& state;
    juce::dsp::ProcessSpec& setup;

    // juce::dsp::DryWetMixer<SampleType> mixer;

    // StoneyDSP::Audio::Biquads<SampleType> biquad;

    /** Unit-delay object(s). */
    std::vector<SampleType> Wn_1, Wn_2, Xn_1, Xn_2, Yn_1, Yn_2;

    /** Coefficient gain(s). */
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
    double sampleRate = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorWrapper)
};

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP

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

// class AudioPluginAudioProcessorParameters;

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

    // //==========================================================================
    // /** Sets the oversampling factor. */
    // void setOversampling();

    // SampleType getLatencySamples() const noexcept;

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

    //==============================================================================
    // std::unique_ptr<juce::dsp::Oversampling<SampleType>> oversampler[5];
    juce::dsp::DryWetMixer<SampleType> mixer;

    //==============================================================================
    /** Unit-delay object(s). */
    std::vector<SampleType> Wn_1, Wn_2, Xn_1, Xn_2, Yn_1, Yn_2;

    /** Coefficient gain(s). */
    StoneyDSP::Maths::Coefficient<SampleType> b0, b1, b2, a0, a1, a2;

    /** Coefficient calculation(s). */
    StoneyDSP::Maths::Coefficient<SampleType> b_0, b_1, b_2, a_0, a_1, a_2;

    //==============================================================================
    /** Parameter pointers. */
    juce::AudioParameterFloat* frequencyPtr;
    juce::AudioParameterFloat* resonancePtr;
    juce::AudioParameterFloat* gainPtr;
    juce::AudioParameterChoice* typePtr;
    juce::AudioParameterChoice* transformPtr;
    // juce::AudioParameterChoice* osPtr;
    juce::AudioParameterFloat* outputPtr;
    juce::AudioParameterFloat* mixPtr;

    filterType filterTypeParamValue { filterType::lowPass2 };
    transformationType transformationParamValue { transformationType::directFormIItransposed };

    bool bypassParamValue { false };

    //==============================================================================
    /** Initialised parameter(s) */
    SampleType
        loop = static_cast<SampleType>(0.0)
        , outputSample = static_cast<SampleType>(0.0)
        , minFreq = static_cast<SampleType>(20.0)
        , maxFreq = static_cast<SampleType>(20000.0)
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

    // int curOS = 0, prevOS = 0, oversamplingFactor = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorWrapper)
};

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP

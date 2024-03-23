/***************************************************************************//**
 * @file Wrapper.hpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief Simple two-pole equalizer with variable oversampling.
 * @version 1.2.3.155
 * @date 2024-03-16
 *
 * @copyright Copyright (c) 2024 - Nathan J. Hood

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

 ******************************************************************************/

#pragma once
#define STONEYDSP_BIQUADS_WRAPPER_HPP_INCLUDED

#include "../Biquads.hpp"

// #ifndef STONEYDSP_BIQUADS_HPP_INCLUDED
// #include <juce_audio_basics/juce_audio_basics.h>                                // depends: juce_core
// #include <juce_audio_formats/juce_audio_formats.h>                              // depends: juce_audio_basics
// #include <juce_audio_processors/juce_audio_processors.h>                        // depends: juce_gui_extra, juce_audio_basics
// #include <juce_core/juce_core.h>                                                //
// #include <juce_data_structures/juce_data_structures.h>                          // depends: juce_events
// #include <juce_dsp/juce_dsp.h>                                                  // depends: juce_audio_formats
// #include <juce_events/juce_events.h>                                            // depends: juce_core
// #include <juce_graphics/juce_graphics.h>                                        // depends: juce_events
// #include <juce_gui_basics/juce_gui_basics.h>                                    // depends: juce_graphics, juce_data_structures
// #include <juce_gui_extra/juce_gui_extra.h>                                      // depends: juce_gui_basics

// #include <stoneydsp_audio/stoneydsp_audio.h>
// #include <stoneydsp_core/stoneydsp_core.h>
// #endif // STONEYDSP_BIQUADS_HPP_INCLUDED

namespace StoneyDSP {
/** @addtogroup StoneyDSP @{ */

namespace Biquads {
/** @addtogroup Biquads @{ */

// class AudioPluginAudioProcessor;

template <typename SampleType>
class AudioPluginAudioProcessorWrapper
{
public:
    //==============================================================================
    /**
     * @brief Construct a new ```StoneyDSP::Audio::AudioPluginAudioProcessorWrapper``` object.
     *
     * @param p
     * @param apvts
     * @param spec
     */
    AudioPluginAudioProcessorWrapper(AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts, juce::dsp::ProcessSpec& spec);
    ~AudioPluginAudioProcessorWrapper();
    //==============================================================================
    /**
     * @brief Initialises the processor.
     *
     * @param spec
     */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();
    void reset(SampleType initialValue = { 0.0 });
    void snapToZero() noexcept;

    //==============================================================================
    /**
     * @brief
     *
     * @param buffer
     * @param midiMessages
     */
    void process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages);
    /**
     * @brief
     *
     * @param buffer
     * @param midiMessages
     */
    void processBlock(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages);
    /**
     * @brief
     *
     * @param buffer
     * @param midiMessages
     */
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

    // void applyGain(juce::AudioBuffer<SampleType>& buffer, SampleType gainLevel = static_cast<SampleType>(1.0))
    // {
    //     for (auto channel = 0; channel < audioProcessor.getTotalNumOutputChannels(); ++channel)
    //         buffer.applyGain (channel, 0, buffer.getNumSamples(), gainLevel);
    // }

    //==============================================================================
    /** Updates the internal state variables of the processor. */
    void update();

    //==============================================================================
    /** Sets the oversampling factor. */
    void setOversampling();

    SampleType getLatencySamples() const noexcept;

private:
    //==============================================================================
    AudioPluginAudioProcessorWrapper() = delete;

    //==============================================================================
    // This reference is provided as a quick way for the wrapper to
    // access the processor object that created it.
    AudioPluginAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& state;
    juce::dsp::ProcessSpec& setup;

    //==============================================================================
    // std::unique_ptr<juce::dsp::Oversampling<SampleType>> oversampler[5];
    std::unique_ptr<juce::dsp::DryWetMixer<SampleType>> mixer;

    const std::size_t biquadArraySize;
    std::unique_ptr<StoneyDSP::Audio::Biquads<SampleType>> biquadArray[4];

    //==========================================================================
    /** Parameter pointers. */
    juce::AudioParameterBool*       masterBypassPtr         { nullptr };
    juce::AudioParameterFloat*      masterOutputPtr         { nullptr };
    juce::AudioParameterFloat*      masterMixPtr            { nullptr };
    juce::AudioParameterChoice*     masterOsPtr             { nullptr };
    juce::AudioParameterChoice*     masterTransformPtr      { nullptr };

    // juce::AudioParameterBool*       biquadsABypassPtr       { nullptr };
    // juce::AudioParameterFloat*      biquadsAFrequencyPtr    { nullptr };
    // juce::AudioParameterFloat*      biquadsAResonancePtr    { nullptr };
    // juce::AudioParameterFloat*      biquadsAGainPtr         { nullptr };
    // juce::AudioParameterChoice*     biquadsATypePtr         { nullptr };

    // juce::AudioParameterBool*       biquadsBBypassPtr       { nullptr };
    // juce::AudioParameterFloat*      biquadsBFrequencyPtr    { nullptr };
    // juce::AudioParameterFloat*      biquadsBResonancePtr    { nullptr };
    // juce::AudioParameterFloat*      biquadsBGainPtr         { nullptr };
    // juce::AudioParameterChoice*     biquadsBTypePtr         { nullptr };

    // juce::AudioParameterBool*       biquadsCBypassPtr       { nullptr };
    // juce::AudioParameterFloat*      biquadsCFrequencyPtr    { nullptr };
    // juce::AudioParameterFloat*      biquadsCResonancePtr    { nullptr };
    // juce::AudioParameterFloat*      biquadsCGainPtr         { nullptr };
    // juce::AudioParameterChoice*     biquadsCTypePtr         { nullptr };

    // juce::AudioParameterBool*       biquadsDBypassPtr       { nullptr };
    // juce::AudioParameterFloat*      biquadsDFrequencyPtr    { nullptr };
    // juce::AudioParameterFloat*      biquadsDResonancePtr    { nullptr };
    // juce::AudioParameterFloat*      biquadsDGainPtr         { nullptr };
    // juce::AudioParameterChoice*     biquadsDTypePtr         { nullptr };

    juce::AudioParameterBool*       bypassState             { nullptr };

    juce::AudioParameterBool* biquadsBypassPtrArray[4];
    juce::AudioParameterFloat* biquadsFrequencyPtrArray[4];
    juce::AudioParameterFloat* biquadsResonancePtrArray[4];
    juce::AudioParameterFloat* biquadsGainPtrArray[4];

    juce::AudioParameterChoice* biquadsFilterTypePtrArray[4];

    //==============================================================================
    /** Initialised constant */
    double sampleRate = 0.0;

    // int curOS = 0, prevOS = 0, oversamplingFactor = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorWrapper)
};

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP

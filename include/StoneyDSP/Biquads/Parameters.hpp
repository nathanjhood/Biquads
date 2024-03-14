/**
 * @file Parameters.hpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief
 * @version 1.2.0
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#define STONEYDSP_BIQUADS_PARAMETERS_HPP_INCLUDED

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

class AudioPluginAudioProcessorParameters
{
public:
    //==========================================================================
    /** Constructor. */
    AudioPluginAudioProcessorParameters(AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);

    //==========================================================================
    /** Create Parameter Layout. */
    static void setParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& params);


private:
    //==========================================================================
    // This reference is provided as a quick way for the wrapper to
    // access the processor object that created it.
    AudioPluginAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorParameters)
};

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP

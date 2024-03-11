/**
 * @file PluginParameters.h
 * @author StoneyDSP (nathanjhood@googlemail.com)
 * @brief
 * @version 0.1
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#define __STONEYDSP_BIQUADS_PLUGINPARAMETERS_H__

#include <JuceHeader.h>

namespace StoneyDSP {
namespace Biquads {

class AudioPluginAudioProcessor;

class AudioPluginAudioProcessorParameters
{
public:
    //==========================================================================
    /** Constructor. */
    AudioPluginAudioProcessorParameters(AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts, juce::dsp::ProcessSpec& spec);

    //==========================================================================
    /** Create Parameter Layout. */
    static void setParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& params);

private:
    //==========================================================================
    // This reference is provided as a quick way for the wrapper to
    // access the processor object that created it.
    AudioPluginAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& state;
    juce::dsp::ProcessSpec& setup;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorParameters)
};

} // namespace StoneyDSP
} // namespace Biquads

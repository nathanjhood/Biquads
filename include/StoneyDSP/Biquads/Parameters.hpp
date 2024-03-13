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

    float   getFrequency()  { return frequencyPtr->get(); }
    float   getResonance()  { return resonancePtr->get(); }
    float   getGain()       { return gainPtr->get(); }
    int     getType()       { return typePtr->getIndex(); }
    int     getTransform()  { return transformPtr->getIndex(); }
    // int     getOs()         { return osPtr->get(); }
    float   getOutput()     { return outputPtr->get(); }
    // float   getMix()        { return mixPtr->get(); }
    bool    getBypass()     { return bypassPtr->get(); }

private:
    //==========================================================================
    juce::AudioParameterFloat* getFrequencyPtr()    { return frequencyPtr; }
    juce::AudioParameterFloat* getResonancePtr()    { return resonancePtr; }
    juce::AudioParameterFloat* getGainPtr()         { return gainPtr; }
    juce::AudioParameterChoice* getTypePtr()        { return typePtr; }
    juce::AudioParameterChoice* getTransformPtr()   { return transformPtr; }
    // juce::AudioParameterChoice* getOsPtr()       { return osPtr; }
    juce::AudioParameterFloat* getOutputPtr()       { return outputPtr; }
    // juce::AudioParameterFloat* getMixPtr()       { return mixPtr; }
    juce::AudioParameterBool* getBypassPtr()        { return bypassPtr; }

    //==========================================================================
    // This reference is provided as a quick way for the wrapper to
    // access the processor object that created it.
    AudioPluginAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& state;

    //==========================================================================
    /** Parameter pointers. */
    juce::AudioParameterFloat*      frequencyPtr    { nullptr };
    juce::AudioParameterFloat*      resonancePtr    { nullptr };
    juce::AudioParameterFloat*      gainPtr         { nullptr };
    juce::AudioParameterChoice*     typePtr         { nullptr };
    juce::AudioParameterChoice*     transformPtr    { nullptr };
    // juce::AudioParameterChoice*  osPtr           { nullptr };
    juce::AudioParameterFloat*      outputPtr       { nullptr };
    // juce::AudioParameterFloat*   mixPtr          { nullptr };
    juce::AudioParameterBool*       bypassPtr       { nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorParameters)
};

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP

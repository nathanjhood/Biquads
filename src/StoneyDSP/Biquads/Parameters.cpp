/**
 * @file Parameters.cpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief
 * @version 1.2.0
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "StoneyDSP/Biquads/Parameters.hpp"
#include "StoneyDSP/Biquads/Processor.hpp"

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

AudioPluginAudioProcessorParameters::AudioPluginAudioProcessorParameters(AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts)
: audioProcessor (p)
, state(apvts)
{
}

void AudioPluginAudioProcessorParameters::setParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& params)
{
    const auto dBMax = juce::Decibels::gainToDecibels(16.0f);
    const auto dBMin = juce::Decibels::gainToDecibels(0.0625f);
    const auto dBOut = juce::Decibels::gainToDecibels(0.5f, -120.0f) * 20.0f;

    const auto freqRange = juce::NormalisableRange<float>(20.00f, 20000.00f, 0.001f, 00.198894f);
    const auto resRange = juce::NormalisableRange<float>(00.00f, 1.00f, 0.01f, 1.00f);
    const auto gainRange = juce::NormalisableRange<float>(dBMin, dBMax, 0.01f, 1.00f);
    const auto mixRange = juce::NormalisableRange<float>(00.00f, 100.00f, 0.01f, 1.00f);
    const auto outputRange = juce::NormalisableRange<float>(dBOut, dBMax, 0.01f, 1.00f);

    const auto fString = juce::StringArray({ "LP2", "LP1", "HP2", "HP1" , "BP2", "BP2c", "LS2", "LS1c", "LS1", "HS2", "HS1c", "HS1", "PK2", "NX2", "AP2" });
    const auto tString = juce::StringArray({ "DFI", "DFII", "DFI t", "DFII t" });
    // const auto osString = juce::StringArray({ "--", "2x", "4x", "8x", "16x" });

    const auto decibels = juce::String{ ("dB") };
    const auto frequency = juce::String{ ("Hz") };
    const auto reso = juce::String{ ("q") };
    const auto percentage = juce::String{ ("%") };

    auto genParam = juce::AudioProcessorParameter::genericParameter;
    auto inMeter = juce::AudioProcessorParameter::inputMeter;
    auto outParam = juce::AudioProcessorParameter::outputGain;
    auto outMeter = juce::AudioProcessorParameter::outputMeter;

    juce::ignoreUnused(inMeter);
    juce::ignoreUnused(outMeter);

    auto freqAttributes = juce::AudioParameterFloatAttributes()
        .withLabel(frequency)
        .withCategory(genParam);

    auto resoAttributes = juce::AudioParameterFloatAttributes()
        .withLabel(reso)
        .withCategory(genParam);

    auto gainAttributes = juce::AudioParameterFloatAttributes()
        .withLabel(decibels)
        .withCategory(genParam);

    auto mixAttributes = juce::AudioParameterFloatAttributes()
        .withLabel(percentage)
        .withCategory(genParam);

    auto outputAttributes = juce::AudioParameterFloatAttributes()
        .withLabel(decibels)
        .withCategory(outParam);

    params.add(
        //======================================================================
        std::make_unique<juce::AudioProcessorParameterGroup>("Master_ID", "0", "seperatorA",
            //==================================================================
              std::make_unique<juce::AudioParameterBool>  (juce::ParameterID{ "Master_bypassID", ProjectInfo::versionNumber}, "Bypass", false)
            , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "Master_outputID", ProjectInfo::versionNumber}, "Output", outputRange, 00.00f, outputAttributes)
            , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "Master_mixID", ProjectInfo::versionNumber}, "Mix", mixRange, 100.00f, mixAttributes)
            // , std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "Master_osID", ProjectInfo::versionNumber}, "Oversampling", osString, 0)
            , std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "Master_transformID", ProjectInfo::versionNumber}, "Transform", tString, 3)
        )
    );

    params.add(
        //======================================================================
        std::make_unique<juce::AudioProcessorParameterGroup>("Band_A_ID", "1", "seperatorB",
            //==================================================================
              std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "Band_A_frequencyID", ProjectInfo::versionNumber}, "Frequency", freqRange, 632.455f, freqAttributes)
            , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "Band_A_resonanceID", ProjectInfo::versionNumber}, "Resonance", resRange, 00.10f, resoAttributes)
            , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "Band_A_gainID", ProjectInfo::versionNumber}, "Gain", gainRange, 00.00f, gainAttributes)
            , std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "Band_A_typeID", ProjectInfo::versionNumber}, "Type", fString, 0)
            //==================================================================
        )
    );

    // params.add(
    //     //======================================================================
    //     std::make_unique<juce::AudioProcessorParameterGroup>("Band_B_ID", "2", "seperatorC",
    //         //==================================================================
    //           std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "Band_B_frequencyID", ProjectInfo::versionNumber}, "Frequency", freqRange, 632.455f, freqAttributes)
    //         , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "Band_B_resonanceID", ProjectInfo::versionNumber}, "Resonance", resRange, 00.10f, resoAttributes)
    //         , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "Band_B_gainID", ProjectInfo::versionNumber}, "Gain", gainRange, 00.00f, gainAttributes)
    //         , std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "Band_B_typeID", ProjectInfo::versionNumber}, "Type", fString, 0)
    //         //==================================================================
    //     )
    // );

    // params.add(
    //     //======================================================================
    //     std::make_unique<juce::AudioProcessorParameterGroup>("Band_C_ID", "3", "seperatorD",
    //         //==================================================================
    //           std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "Band_C_frequencyID", ProjectInfo::versionNumber}, "Frequency", freqRange, 632.455f, freqAttributes)
    //         , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "Band_C_resonanceID", ProjectInfo::versionNumber}, "Resonance", resRange, 00.10f, resoAttributes)
    //         , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "Band_C_gainID", ProjectInfo::versionNumber}, "Gain", gainRange, 00.00f, gainAttributes)
    //         , std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "Band_C_typeID", ProjectInfo::versionNumber}, "Type", fString, 0)
    //         //==================================================================
    //     )
    // );
}

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP
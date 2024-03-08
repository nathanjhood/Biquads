/**
 * @file PluginEditor.hpp
 * @author StoneyDSP (nathanjhood@googlemail.com)
 * @brief
 * @version 0.1
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "PluginProcessor.hpp"
// #include "Components/AutoComponent.hpp"

//==============================================================================
class BiquadsAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit BiquadsAudioProcessorEditor (BiquadsAudioProcessor&);
    ~BiquadsAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BiquadsAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BiquadsAudioProcessorEditor)
};

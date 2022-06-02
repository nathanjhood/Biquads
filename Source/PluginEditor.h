/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Components/AutoComponent.h"

//==============================================================================
/**
*/
class BiquadsAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

    //==========================================================================
    BiquadsAudioProcessorEditor (BiquadsAudioProcessor& p);
    ~BiquadsAudioProcessorEditor() override;

    //==========================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BiquadsAudioProcessor& audioProcessor;
    APVTS& apvts;

    AutoComponent subComponents;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BiquadsAudioProcessorEditor)
};

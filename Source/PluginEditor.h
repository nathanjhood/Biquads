/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
//#include "Components/AutoKnob.h"
//#include "Components/AutoButton.h"
//#include "Components/AutoComboBox.h"
#include "Components/AutoComponent.h"

//==============================================================================
/**
*/
class BiquadsAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

    //==========================================================================
    BiquadsAudioProcessorEditor (BiquadsAudioProcessor& p, APVTS& apvts, juce::UndoManager& um);
    ~BiquadsAudioProcessorEditor() override;

    //==========================================================================
    void timerCallback() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BiquadsAudioProcessor& audioProcessor;
    APVTS& state;
    juce::UndoManager& undoManager;

    /*AutoKnob knobComponents;
    AutoButton buttonComponents;
    AutoComboBox comboBoxComponents;*/
    AutoComponent subComponents;
    juce::TextButton undoButton, redoButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BiquadsAudioProcessorEditor)
};

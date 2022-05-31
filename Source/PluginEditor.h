/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class BiquadsAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

    //==========================================================================
    BiquadsAudioProcessorEditor (BiquadsAudioProcessor& p, APVTS& apvts);
    ~BiquadsAudioProcessorEditor() override;

    //==========================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BiquadsAudioProcessor& audioProcessor;
    APVTS& state;

    juce::ToggleButton ioButton;
    juce::Label ioLabel;
    std::unique_ptr<APVTS::ButtonAttachment> ioAttach;

    juce::Slider freqSlider;
    juce::Label freqLabel;
    std::unique_ptr<APVTS::SliderAttachment> freqAttach;

    juce::Slider resSlider;
    juce::Label resLabel;
    std::unique_ptr<APVTS::SliderAttachment> resAttach;

    juce::Slider gainSlider;
    juce::Label gainLabel;
    std::unique_ptr<APVTS::SliderAttachment> gainAttach;

    juce::ComboBox typeBox;
    juce::Label typeLabel;
    std::unique_ptr<APVTS::ComboBoxAttachment> typeAttach;

    juce::ComboBox osBox;
    juce::Label osLabel;
    std::unique_ptr<APVTS::ComboBoxAttachment> osAttach;

    juce::Slider outputSlider;
    juce::Label outputLabel;
    std::unique_ptr<APVTS::SliderAttachment> outputAttach;

    juce::Slider dryWetSlider;
    juce::Label dryWetLabel;
    std::unique_ptr<APVTS::SliderAttachment> dryWetAttach;

    juce::ToggleButton bypassButton;
    juce::Label bypassLabel;
    std::unique_ptr<APVTS::ButtonAttachment> bypassAttach;

    juce::ToggleButton displayButton;
    juce::Label displayLabel;
    std::unique_ptr<APVTS::ButtonAttachment> displayAttach;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BiquadsAudioProcessorEditor)
};

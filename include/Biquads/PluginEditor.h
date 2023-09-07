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

#define __STONEYDSP_BIQUADS_PLUGINEDITOR_H__

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "Components/AutoComponent.h"

/**
 * @brief The 'BiquadsAudioProcessorEditor' class.
 *
 */
class BiquadsAudioProcessorEditor   : public juce::AudioProcessorEditor
                                    , public juce::Timer
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    using Undo = juce::UndoManager;

    BiquadsAudioProcessorEditor (BiquadsAudioProcessor& p);
    ~BiquadsAudioProcessorEditor () override;


    void timerCallback () override;
    void paint (juce::Graphics&) override;
    void resized () override;

private:

    /**
     * @brief This reference is provided as a quick way for your editor to
     * access the processor object that created it.
     */
    BiquadsAudioProcessor& audioProcessor;

    APVTS& state;

    Undo& undoManager;

    AutoComponent subComponents;

    // stoneydsp::graphics::VerticalMeter meterLeft, meterRight;

    juce::ArrowButton undoButton { "Undo", 0.5f , juce::Colours::white };
    juce::ArrowButton redoButton { "Redo", 0.0f , juce::Colours::white };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BiquadsAudioProcessorEditor)
};

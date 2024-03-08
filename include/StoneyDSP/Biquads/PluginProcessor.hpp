/**
 * @file PluginProcessor.hpp
 * @author StoneyDSP (nathanjhood@googlemail.com)
 * @brief
 * @version 0.1
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */


#pragma once

// #include <juce_audio_processors/juce_audio_processors.h>

#include <JuceHeader.h>

// #include "PluginParameters.h"
// #include "PluginWrapper.h"

//==============================================================================
class BiquadsAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    BiquadsAudioProcessor();
    ~BiquadsAudioProcessor() override;

    //==========================================================================
    juce::AudioProcessorParameter* getBypassParameter() const override;
    bool isBypassed() const noexcept;
    void setBypassParameter(juce::AudioParameterBool* newBypass) noexcept;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void getCurrentProgramStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void setCurrentProgramStateInformation(const void* data, int sizeInBytes) override;

    //==========================================================================
    /** Undo Manager. */
    juce::UndoManager undoManager;
    /**
     * @brief Get a reference to the UndoManager object.
     *
     * @return juce::UndoManager&
     */
    juce::UndoManager& getUndoManager() { return undoManager; };

    //==========================================================================
    /** Audio processor value tree. */
    juce::AudioProcessorValueTreeState apvts;
    /**
     * @brief Get a reference to the AudioProcessorValueTreeState object.
     *
     * @return juce::AudioProcessorValueTreeState&
     */
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; };
    /**
     * @brief Create a juce::AudioProcessorValueTreeState::ParameterLayout object.
     *
     * @return juce::AudioProcessorValueTreeState::ParameterLayout
     */
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioParameterBool* bypassState { nullptr };

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BiquadsAudioProcessor)
};

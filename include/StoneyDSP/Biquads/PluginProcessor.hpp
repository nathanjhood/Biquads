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

#define STONEYDSP_BIQUADS_PLUGINPROCESSOR_HPP

#include <JuceHeader.h>

// #include "PluginParameters.h"
// #include "PluginProcessWrapper.hpp"

//==============================================================================
class BiquadsAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    BiquadsAudioProcessor();
    ~BiquadsAudioProcessor() override;

    //==============================================================================
    juce::AudioProcessorParameter* getBypassParameter() const override;
    bool isBypassed() const noexcept;
    void setBypassParameter(juce::AudioParameterBool* newBypass) noexcept;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

        //==========================================================================
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;
    void processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    void processBlockBypassed(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) override;

    // (TODO: what is this this about?) using AudioProcessor::processBlock;

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

    //==============================================================================
    /** Undo Manager. */
    juce::UndoManager undoManager;
    juce::UndoManager& getUndoManager() { return undoManager; }

    //==============================================================================
    /** Audio processor value tree. */
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
    /** Audio processor specs. */
    juce::dsp::ProcessSpec spec;
    juce::dsp::ProcessSpec& getSpec() { return spec; }

private:
    //==============================================================================
    /** Audio processor members. */
    // ProcessWrapper<float> processorFloat;
    // ProcessWrapper<double> processorDouble;
    juce::AudioParameterBool* bypassState { nullptr };
    juce::AudioParameterFloat* outputPtr  { nullptr };
    // Parameters parameters;

    StoneyDSP::Maths::Coefficient<float> coeffFlt;
    StoneyDSP::Maths::Coefficient<double> coeffDbl;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BiquadsAudioProcessor)
};

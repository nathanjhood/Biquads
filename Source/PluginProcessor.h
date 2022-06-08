/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once


#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginParameters.h"
#include "PluginWrapper.h"

//==============================================================================
/**
*/
class BiquadsAudioProcessor  : public juce::AudioProcessor
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    using precisionType = ProcessingPrecision;
    //==========================================================================
    BiquadsAudioProcessor();
    ~BiquadsAudioProcessor() override;

    //==========================================================================
    juce::AudioProcessorParameter* getBypassParameter() const;
    bool supportsDoublePrecisionProcessing() const override;
    ProcessingPrecision getProcessingPrecision() const noexcept;
    bool isUsingDoublePrecision() const noexcept;
    void setProcessingPrecision(ProcessingPrecision newPrecision) noexcept;
    
    //==========================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void numChannelsChanged() override;
    void numBusesChanged() override;
    void processorLayoutsChanged() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    //==========================================================================
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;
    void processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    void processBlockBypassed(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) override;

    //==========================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==========================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==========================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==========================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==========================================================================
    /** Audio processor value tree. */
    juce::UndoManager undoManager;
    APVTS apvts;
    APVTS& getAPVTS() { return apvts; };
    static APVTS::ParameterLayout createParameterLayout();
    
private:
    //==========================================================================
    /** Audio processor members. */
    Parameters parameters { *this, getAPVTS() };
    ProcessWrapper<float> processorFloat { *this, getAPVTS() };
    ProcessWrapper<double> processorDouble { *this, getAPVTS() };

    //==========================================================================
    /** Parameter pointers. */
    juce::AudioParameterInt* precisionPtr { nullptr };
    juce::AudioParameterBool* bypassPtr { nullptr };

    //==========================================================================
    /** Init variables. */
    precisionType processingPrecision = precisionType::singlePrecision;

    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BiquadsAudioProcessor)
};

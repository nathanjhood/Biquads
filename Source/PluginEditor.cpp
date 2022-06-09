/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BiquadsAudioProcessorEditor::BiquadsAudioProcessorEditor (BiquadsAudioProcessor& p, APVTS& apvts, juce::UndoManager& um)
    : 
    juce::AudioProcessorEditor (&p), 
    audioProcessor (p), 
    state(apvts),
    undoManager(um),
    /*knobComponents(p, apvts),
    buttonComponents(p, apvts),
    comboBoxComponents(p, apvts),*/
    subComponents(p, apvts),
    undoButton("Undo"),
    redoButton("Redo")
{
    setSize(400, 300);
    /*addAndMakeVisible(buttonComponents);
    addAndMakeVisible(knobComponents);
    addAndMakeVisible(comboBoxComponents);*/
    addAndMakeVisible(subComponents);
    addAndMakeVisible(undoButton);
    addAndMakeVisible(redoButton);
    undoButton.onClick = [this] { undoManager.undo(); };
    redoButton.onClick = [this] { undoManager.redo(); };
    setResizable(true, false);

    startTimerHz(24);
}

BiquadsAudioProcessorEditor::~BiquadsAudioProcessorEditor()
{
}

////==============================================================================
void BiquadsAudioProcessorEditor::timerCallback()
{
    /*knobComponents.resized();
    buttonComponents.resized();
    comboBoxComponents.resized();*/
    undoManager.beginNewTransaction();
}

//==============================================================================
void BiquadsAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::black);

    //// draw an outline around the component
    g.setColour(juce::Colours::hotpink);
    g.drawRect(getLocalBounds(), 2);

    // Add project info text to background here
    g.setColour(juce::Colours::antiquewhite);
    g.setFont(15.0f);
    g.drawFittedText(ProjectInfo::companyName, getLocalBounds(), juce::Justification::topLeft, 1);
    g.drawFittedText(ProjectInfo::projectName, getLocalBounds(), juce::Justification::topRight, 1);
    g.drawFittedText(ProjectInfo::versionString, getLocalBounds(), juce::Justification::bottomRight, 1);
}

void BiquadsAudioProcessorEditor::resized()
{
    subComponents.resized();
    subComponents.setBounds(0, 0, getWidth(), getHeight());
}

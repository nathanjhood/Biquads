/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BiquadsAudioProcessorEditor::BiquadsAudioProcessorEditor (BiquadsAudioProcessor& p)
    : 
    juce::AudioProcessorEditor (&p), 
    audioProcessor (p), 
    state(p.getAPVTS()),
    undoManager(p.getUndoManager()),
    subComponents(p, p.getAPVTS()),
    meterLeft(), meterRight()
{
    setSize(430, 300);
    addAndMakeVisible(subComponents);
    addAndMakeVisible(meterLeft);
    addAndMakeVisible(meterRight);
    addAndMakeVisible(undoButton);
    addAndMakeVisible(redoButton);
    undoButton.onClick = [this] { audioProcessor.getUndoManager().undo(); };
    redoButton.onClick = [this] { audioProcessor.getUndoManager().redo(); };
    setResizable(true, true);

    startTimerHz(60);
}

BiquadsAudioProcessorEditor::~BiquadsAudioProcessorEditor()
{
}

////==============================================================================
void BiquadsAudioProcessorEditor::timerCallback()
{
    meterLeft.setLevel(audioProcessor.getRMSLevel(0));
    meterRight.setLevel(audioProcessor.getRMSLevel(1));

    meterLeft.repaint();
    meterRight.repaint();
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
    g.drawFittedText(ProjectInfo::versionString, getLocalBounds(), juce::Justification::bottomLeft, 1);
}

void BiquadsAudioProcessorEditor::resized()
{
    subComponents.setBounds(0, 0, getWidth(), getHeight());
    meterLeft.setBounds(130, 20, 200, 15);
    meterRight.setBounds(130, 40, 200, 15);
    undoButton.setBounds((getWidth() / 2) - 10, getHeight() - 20, 20, 20);
    redoButton.setBounds((getWidth() / 2) + 10, getHeight() - 20, 20, 20);

    subComponents.resized();
    meterLeft.resized();
    meterRight.resized();
    undoButton.resized();
    redoButton.resized();
}

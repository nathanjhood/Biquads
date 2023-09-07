/**
 * @file PluginEditor.cpp
 * @author StoneyDSP (nathanjhood@googlemail.com)
 * @brief
 * @version 0.1
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

/**
 * @brief Construct a new 'BiquadsAudioProcessorEditor' object.
 *
 * @param p
 */
BiquadsAudioProcessorEditor::BiquadsAudioProcessorEditor(BiquadsAudioProcessor& p)
    :   juce::AudioProcessorEditor(&p)
    ,   audioProcessor(p)
    ,   state(p.getAPVTS())
    ,   undoManager(p.getUndoManager())
    ,   subComponents(p, p.getAPVTS())
    // ,   meterLeft(([&]() { return audioProcessor.getRMSLevel(0); }))
    // ,   meterRight(([&]() { return audioProcessor.getRMSLevel(1); }))
{
    setSize(500, 300);
    addAndMakeVisible(subComponents);
    // addAndMakeVisible(meterLeft);
    // addAndMakeVisible(meterRight);
    addAndMakeVisible(undoButton);
    addAndMakeVisible(redoButton);
    undoButton.onClick = [this] { audioProcessor.getUndoManager().undo(); };
    redoButton.onClick = [this] { audioProcessor.getUndoManager().redo(); };
    setResizable(true, true);

    startTimerHz(60);
}

/**
 * @brief Destroy the 'BiquadsAudioProcessorEditor' object.
 *
 */
BiquadsAudioProcessorEditor::~BiquadsAudioProcessorEditor()
{
}

/**
 * @brief the 'BiquadsAudioProcessorEditor::timerCallback' method.
 *
 */
void BiquadsAudioProcessorEditor::timerCallback()
{
}

/**
 * @brief the 'BiquadsAudioProcessorEditor::paint' method.
 *
 * @param g
 */
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


/**
 * @brief The 'BiquadsAudioProcessorEditor::resized' method.
 *
 */
void BiquadsAudioProcessorEditor::resized()
{
    subComponents.setBounds(0, 0, getWidth(), getHeight());
    //meterLeft.setBounds(130, 20, 200, 15);
    //meterRight.setBounds(130, 40, 200, 15);
    // meterLeft.setBounds(420, 50, 15, 200);
    // meterRight.setBounds(440, 50, 15, 200);
    undoButton.setBounds((getWidth() / 2) - 10, getHeight() - 20, 20, 20);
    redoButton.setBounds((getWidth() / 2) + 10, getHeight() - 20, 20, 20);

    subComponents.resized();
    // meterLeft.resized();
    // meterRight.resized();
    undoButton.resized();
    redoButton.resized();
}

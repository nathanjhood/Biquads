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
    knobComponents(p, apvts),
    buttonComponents(p, apvts),
    comboBoxComponents(p, apvts)
{
    setSize(500, 600);
    addAndMakeVisible(buttonComponents);
    addAndMakeVisible(knobComponents);
    addAndMakeVisible(comboBoxComponents);
    setResizable(true, false);

    startTimerHz(24);
}

BiquadsAudioProcessorEditor::~BiquadsAudioProcessorEditor()
{
}

////==============================================================================
void BiquadsAudioProcessorEditor::timerCallback()
{
    knobComponents.resized();
    buttonComponents.resized();
    comboBoxComponents.resized();
}

//==============================================================================
void BiquadsAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::darkslategrey);

    //// draw an outline around the component
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds(), 5);

    // Add project info text to background here
    g.setColour(juce::Colours::antiquewhite);
    g.setFont(15.0f);
    g.drawFittedText(ProjectInfo::companyName, getLocalBounds(), juce::Justification::topLeft, 1);
    g.drawFittedText(ProjectInfo::projectName, getLocalBounds(), juce::Justification::topRight, 1);
    g.drawFittedText(ProjectInfo::versionString, getLocalBounds(), juce::Justification::bottomRight, 1);
}

void BiquadsAudioProcessorEditor::resized()
{
    auto width = getWidth();
    auto height = getHeight();
    auto bounds = getBounds();
    auto left = 0;
    auto right = getRight();
    auto bottom = getBottom();

    // Positions...

    auto absCentreX = getWidth() / 3;
    auto absCentreY = getHeight() / 3;

    auto absLeftX = 0;
    auto absRightX = getRight();

    auto absTopY = 0;
    auto absBottomY = getBottom();

    // Sizes...

    auto wholeW = getWidth();
    auto wholeY = getHeight();

    auto halfW = getWidth() / 2;
    auto halfH = getHeight() / 2;

    auto thirdW = getWidth() / 3;
    auto thirdH = getHeight() / 3;

    knobComponents.setBounds(left, absCentreY, wholeW, thirdH);
    buttonComponents.setBounds(halfW, 1 / bottom, halfW, height / 4);
    comboBoxComponents.setBounds(halfW, bottom - (halfH / 2), halfW, height / 4);
}

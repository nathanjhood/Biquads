/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BiquadsAudioProcessorEditor::BiquadsAudioProcessorEditor (BiquadsAudioProcessor& p, APVTS& apvts)
    : AudioProcessorEditor (&p), audioProcessor (p), state(apvts)
{

    addAndMakeVisible(freqSlider);
    freqAttach.reset(new APVTS::SliderAttachment(state, "frequencyID", freqSlider));

    addAndMakeVisible(resSlider);
    resAttach.reset(new APVTS::SliderAttachment(state, "resonanceID", resSlider));

    addAndMakeVisible(gainSlider);
    gainAttach.reset(new APVTS::SliderAttachment(state, "gainID", gainSlider));

    setSize (400, 300);
}

BiquadsAudioProcessorEditor::~BiquadsAudioProcessorEditor()
{
}

//==============================================================================
void BiquadsAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("StoneyDSP", getLocalBounds(), juce::Justification::topLeft, 1);
    g.drawFittedText (audioProcessor.getName(), getLocalBounds(), juce::Justification::topRight, 1);
}

void BiquadsAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto sliderLeft = 30;
    freqSlider.setBounds(sliderLeft, 30, getWidth() - sliderLeft - 10, 20);
    resSlider.setBounds(sliderLeft, 70, getWidth() - sliderLeft - 10, 20);
    gainSlider.setBounds(sliderLeft, 110, getWidth() - sliderLeft - 10, 20);
}

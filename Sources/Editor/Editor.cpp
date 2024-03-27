/***************************************************************************//**
 * @file Editor.cpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief Simple two-pole equalizer with variable oversampling.
 * @version 1.2.3.155
 * @date 2024-03-16
 *
 * @copyright Copyright (c) 2024 - Nathan J. Hood

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

 ******************************************************************************/

// #ifdef STONEYDSP_BIQUADS_EDITOR_HPP_INCLUDED
//  #error "Incorrect usage of 'Editor.cpp'!"
// #endif

#include "Editor.hpp"

namespace StoneyDSP {
/** @addtogroup StoneyDSP @{ */

namespace Biquads {
/** @addtogroup Biquads @{ */

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
: juce::AudioProcessorEditor(&p)
, audioProcessor(p)
, undoManager(p.getUndoManager())
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(500, 300);
    addAndMakeVisible(undoButton);
    addAndMakeVisible(redoButton);
    undoButton.onClick = [this] { audioProcessor.getUndoManager().undo(); };
    redoButton.onClick = [this] { audioProcessor.getUndoManager().redo(); };
    setResizable(true, true);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::hotpink);
    g.drawRect(getLocalBounds(), 2);

    // Add project info text to background here
    g.setColour(juce::Colours::antiquewhite);
    g.setFont(15.0f);
    g.drawFittedText(ProjectInfo::companyName, getLocalBounds(), juce::Justification::topLeft, 1);
    g.drawFittedText(ProjectInfo::projectName, getLocalBounds(), juce::Justification::topRight, 1);
    g.drawFittedText(ProjectInfo::versionString, getLocalBounds(), juce::Justification::bottomLeft, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    undoButton.setBounds((getWidth() / 2) - 10, getHeight() - 20, 20, 20);
    redoButton.setBounds((getWidth() / 2) + 10, getHeight() - 20, 20, 20);

    undoButton.resized();
    redoButton.resized();
}

  /** @} group Biquads */
} // namespace Biquads

  /** @} group StoneyDSP */
} // namespace StoneyDSP

/*
  ==============================================================================

    AutoComboBox.cpp
    Created: 30 May 2022 2:54:47pm
    Author: StoneyDSP

  ==============================================================================
*/

#ifndef AUTOCOMBOBOX_H_INCLUDED
#define AUTOCOMBOBOX_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

/*
  ==============================================================================

    Look and Feel.

  ==============================================================================
*/

class AutoComboBoxLookAndFeel : public juce::LookAndFeel_V4
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

    //==========================================================================
    /** Constructor. */
    AutoComboBoxLookAndFeel();
};

/*
  ==============================================================================

    Define parameter objects.

  ==============================================================================
*/

using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

struct BoxWithAttachment
{
    juce::ComboBox box;
    std::unique_ptr<ComboBoxAttachment> attachment;
};

/*
  ==============================================================================

    AutoComboBox.

  ==============================================================================
*/

class AutoComboBox : public juce::Component
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    //==========================================================================
    /** Constructor. */
    AutoComboBox(juce::AudioProcessor& p, APVTS& apvts, std::function<void()> paramLambda = {});
    ~AutoComboBox() override;

    //==========================================================================
    /** Component methods. */
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    //==========================================================================
    /** Instantiate members. */
    AutoComboBoxLookAndFeel lookAndfeel;
    juce::OwnedArray<BoxWithAttachment> boxes;

    std::function<void()> lambdaSupplier;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutoComboBox)
};

#endif //AUTOCOMBOBOX_H_INCLUDED
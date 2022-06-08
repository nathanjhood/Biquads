/*
  ==============================================================================

    AutoButton.h
    Created: 5 Jun 2022 3:21:48am
    Author:  StoneyDSP

  ==============================================================================
*/

#ifndef AUTOBUTTON_H_INCLUDED
#define AUTOBUTTON_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/

/*
  ==============================================================================

    Look and Feel.

  ==============================================================================
*/

class AutoButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

    //==========================================================================
    /** Constructor. */
    AutoButtonLookAndFeel();
};

using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

struct ButtonWithAttachment
{
    juce::TextButton button;
    std::unique_ptr<ButtonAttachment> attachment;
};

class AutoButton  : public juce::Component
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    using Lambda = const std::function<void()>;
    //==========================================================================
    /** Constructor. */
    AutoButton(juce::AudioProcessor& p, APVTS& apvts, Lambda& paramLambda = {});
    ~AutoButton() override;

    //==========================================================================
    /** Component methods. */
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    Lambda& lambdaSupplier;
    //==========================================================================
    /** Instantiate members. */
    AutoButtonLookAndFeel lookAndfeel;
    juce::OwnedArray<ButtonWithAttachment> buttons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoButton)
};

#endif //AUTOBUTTON_H_INCLUDED

/*
  ==============================================================================

    AutoComponent.h
    Created: 30 May 2022 2:54:47pm
    Author:  (?) & StoneyDSP

  ==============================================================================
*/

#ifndef AUTOCOMPONENT_H_INCLUDED
#define AUTOCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

//==============================================================================
/** Define parameter objects. */

struct SliderWithAttachment
{
    juce::Slider slider;
    std::unique_ptr<SliderAttachment> attachment;
};

struct BoxWithAttachment
{
    juce::ComboBox box;
    std::unique_ptr<ComboBoxAttachment> attachment;
};

struct ButtonWithAttachment
{
    juce::TextButton button;
    std::unique_ptr<ButtonAttachment> attachment;
};

//==============================================================================
/** Define AutoComponent. */

class AutoComponent : public juce::Component
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    //==========================================================================
    /** Constructor. */
    AutoComponent (juce::AudioProcessor& p, APVTS& apvts, std::function<void()> paramLambda = {});

    //==========================================================================
    /** Component methods. */
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==========================================================================
    /** Instantiate members. */
    juce::OwnedArray<SliderWithAttachment> sliders;
    juce::OwnedArray<BoxWithAttachment> boxes;
    juce::OwnedArray<ButtonWithAttachment> buttons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoComponent)
};

#endif //AUTOCOMPONENT_H_INCLUDED

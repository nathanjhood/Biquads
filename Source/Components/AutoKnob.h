/*
  ==============================================================================

    AutoKnob.h
    Created: 30 May 2022 2:54:47pm
    Author:  StoneyDSP

  ==============================================================================
*/

#ifndef AUTOKNOB_H_INCLUDED
#define AUTOKNOB_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

/*
  ==============================================================================

    Look and Feel.

  ==============================================================================
*/

class AutoKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

    //==========================================================================
    /** Constructor. */
    AutoKnobLookAndFeel();
};

/*
  ==============================================================================

    Define parameter objects.

  ==============================================================================
*/

using KnobAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

struct KnobWithAttachment
{
    juce::Slider knob;
    std::unique_ptr<KnobAttachment> attachment;
};

/*
  ==============================================================================

    AutoKnob.

  ==============================================================================
*/

class AutoKnob : public juce::Component
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    using Lambda = const std::function<void()>;
    //==========================================================================
    /** Constructor. */    
    AutoKnob(juce::AudioProcessor& p, APVTS& apvts, Lambda& paramLambda = {});
    ~AutoKnob() override;

    //==========================================================================
    /** Component methods. */
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    Lambda& lambdaSupplier;

    //==========================================================================
    /** Instantiate members. */
    AutoKnobLookAndFeel lookAndfeel;
    juce::OwnedArray<KnobWithAttachment> knobs;

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutoKnob)
};

#endif //AUTOKNOB_H_INCLUDED
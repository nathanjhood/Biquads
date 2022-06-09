/*
  ==============================================================================

    AutoKnob.cpp
    Created: 30 May 2022 2:54:47pm
    Author: StoneyDSP

  ==============================================================================
*/

#include "AutoKnob.h"

/*
  ==============================================================================

    Look and Feel.

  ==============================================================================
*/

AutoKnobLookAndFeel::AutoKnobLookAndFeel()
{
    ///* Knob style */
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightslategrey);
    //setColour(juce::Slider::backgroundColourId, juce::Colours::brown);
    setColour(juce::Slider::thumbColourId, juce::Colours::hotpink);
    setColour(juce::Slider::trackColourId, juce::Colours::black);
    setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::darkgrey);
    setColour(juce::Slider::textBoxHighlightColourId, juce::Colours::hotpink);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::lightgrey);
}

/*
  ==============================================================================

    AutoKnob.

  ==============================================================================
*/

AutoKnob::AutoKnob(juce::AudioProcessor& p, APVTS& apvts, Lambda& paramLambda) : lambdaSupplier(paramLambda)
{
    auto addKnob = [=, &apvts] (juce::AudioParameterFloat* param)
    {
        KnobWithAttachment* newKnob = new KnobWithAttachment;
        
        addAndMakeVisible(newKnob->knob);
        newKnob->attachment.reset(new KnobAttachment(apvts, param->paramID, newKnob->knob));

        auto suffix = " " + param->getLabel().fromLastOccurrenceOf("_", false, false);
        newKnob->knob.setTextValueSuffix(suffix);

        newKnob->knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        newKnob->knob.setName(param->name);
        //newKnob->knob.textFromValueFunction = valueSupplier;
        newKnob->knob.textFromValueFunction = nullptr; // @TODO: Don't override lambda from VTS
        newKnob->knob.setNumDecimalPlacesToDisplay(2);
        newKnob->knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 16);
        newKnob->knob.onValueChange = paramLambda;

        knobs.add(newKnob);
    };

    auto params = p.getParameters();

    for (auto* param : params)
    {
        //======================================================================
        /** If = ParameterFloat, make new Rotary Slider with Attachment */

        if (auto* paramFloat = dynamic_cast<juce::AudioParameterFloat*> (param))
        {
            addKnob(paramFloat);
            continue;
        }
    }

    setSize(getWidth(), getHeight());
    
}

AutoKnob::~AutoKnob()
{
}

//==============================================================================
void AutoKnob::paint(juce::Graphics& g)
{
    //==========================================================================
    /** Paint Knob border. */

    /*g.setColour(juce::Colours::lightslategrey);
    g.drawRect(getLocalBounds(), 5);*/

    //// Add project info text to background here
    //g.setColour(juce::Colours::antiquewhite);
    //g.setFont(15.0f);
    //g.drawFittedText("Knobs", getLocalBounds(), juce::Justification::centredTop, 1);

    //==========================================================================
    /** Paint Slider name. */

    auto paintName = [this, &g](juce::Component& comp, juce::String name)
    {
        const int height = 20;
        const int initialY = 2;

        juce::Rectangle<int> nameBox(comp.getX(), comp.getY() / 2 /*initialY*/, comp.getWidth(), height);

        g.setColour(juce::Colours::antiquewhite);
        g.setFont(15.0f);
        g.drawFittedText(name, nameBox, juce::Justification::centredBottom, 1);
    };

    for (auto* k : knobs)
        paintName(k->knob, k->knob.getName());

    //==========================================================================
    /** Apply local Look and Feel. */

    auto applyLookAndFeel = [this, &g](juce::Component& comp)
    {
        comp.setLookAndFeel(&lookAndfeel);
    };

    for (auto* k : knobs)
        applyLookAndFeel(k->knob);
}

//==============================================================================

void AutoKnob::resized()
{
    //==========================================================================
    /** This is generally where you'll want to lay out the positions of any
    /** subcomponents in your editor... */
    
    int x = 5;
    bool first = true;

    auto width = getWidth();
    auto height = getHeight();
    auto bounds = getBounds();

    auto absCentreX = getWidth() / 3;
    auto absCentreY = getHeight() / 3;

    for (auto* k : knobs)
    {
        int offset = first ? 0 : 20; // horizontal separation
        k->knob.setBounds(x - offset, absCentreY, 85, 80);
        x = k->knob.getRight();
        first = false;
    }

    //for (auto* k : knobs)
    //{
    //    int offset = first ? 0 : 20; // horizontal separation
    //    k->knob.setBounds(x - offset, 15, 85, 80);
    //    x = k->knob.getRight();
    //    first = false;
    //}
}
//==============================================================================

/*
  ==============================================================================

    AutoComboBox.cpp
    Created: 30 May 2022 2:54:47pm
    Author: StoneyDSP

  ==============================================================================
*/

#include "AutoComboBox.h"

/*
  ==============================================================================

    Look and Feel.

  ==============================================================================
*/

AutoComboBoxLookAndFeel::AutoComboBoxLookAndFeel()
{
    //setColourScheme(LookAndFeel_V4::getMidnightColourScheme());
    /*LookAndFeel_V4::getLightColourScheme();
    LookAndFeel_V4::getDarkColourScheme();
    LookAndFeel_V4::getGreyColourScheme();
    LookAndFeel_V4::getMidnightColourScheme();*/

    //* Button style */
    setColour(juce::Button::buttonDown, juce::Colours::hotpink);
    setColour(juce::Button::buttonNormal, juce::Colours::darkgrey);
    setColour(juce::Button::buttonOver, juce::Colours::lightslategrey);

    ///* Text Button style */
    setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    setColour(juce::TextButton::buttonOnColourId, juce::Colours::hotpink);
    setColour(juce::TextButton::buttonNormal, juce::Colours::darkgrey);
    setColour(juce::TextButton::buttonOver, juce::Colours::lightgrey);
    //setColour(TextButton::buttonDown, juce::Colours::green);
    //setColour(TextButton::textColourOnId, juce::Colours::white);
    //setColour(TextButton::textColourOffId, juce::Colours::transparentWhite);
}

/*
  ==============================================================================

    AutoComboBox.

  ==============================================================================
*/

AutoComboBox::AutoComboBox(juce::AudioProcessor& p, APVTS& apvts, std::function<void()> paramLambda)
{
    auto addBox = [=, &apvts](juce::AudioParameterChoice* param)
    {
        BoxWithAttachment* newBox = new BoxWithAttachment;

        addAndMakeVisible(newBox->box);

        newBox->box.setName(param->name);
        newBox->box.addItemList(param->choices, 1);
        newBox->box.setSelectedItemIndex(0);
        newBox->box.onChange = paramLambda;

        newBox->attachment.reset(new ComboBoxAttachment(apvts, param->paramID, newBox->box));

        boxes.add(newBox);
    };

    auto params = p.getParameters();

    for (auto* param : params)
    {
        //======================================================================
        /** If = ParameterChoice, make new Box with Attachment */

        if (auto* paramChoice = dynamic_cast<juce::AudioParameterChoice*> (param))
        {
            addBox(paramChoice);
            continue;
        }
    }

    setSize(getWidth(), getHeight());
}

AutoComboBox::~AutoComboBox()
{
}

//==============================================================================

void AutoComboBox::paint(juce::Graphics& g)
{
    //==========================================================================
    /** Paint Boxes border. */

    

    //==========================================================================
    /** Paint Box name. */

    auto paintName = [this, &g](juce::Component& comp, juce::String name)
    {
        const int height = 20;
        const int initialY = 2;
        juce::Rectangle<int> nameBox(comp.getX(), initialY, comp.getWidth(), height);
        g.setColour(juce::Colours::antiquewhite);
        g.setFont(15.0f);
        g.drawFittedText(name, nameBox, juce::Justification::centred, 1);
    };

    for (auto* b : boxes)
        paintName(b->box, b->box.getName());

    //==========================================================================
    /** Apply local Look and Feel. */

    auto applyLookAndFeel = [this, &g](juce::Component& comp)
    {
        comp.setLookAndFeel(&lookAndfeel);
    };

    for (auto* b : boxes)
        applyLookAndFeel(b->box);

    //g.setColour(juce::Colours::lightslategrey);
    //g.drawRect(getLocalBounds(), 5);

    //// Add parameter info text to background here
    //g.setColour(juce::Colours::antiquewhite);
    //g.setFont(15.0f);
    //g.drawFittedText("Boxes", getLocalBounds(), juce::Justification::centredBottom, 1);
}

//==============================================================================

void AutoComboBox::resized()
{
    //==========================================================================
    /** This is generally where you'll want to lay out the positions of any
    /** subcomponents in your editor... */

    int x = 20;
    bool first = true;

    auto width = getWidth();
    auto height = getHeight();
    auto bounds = getBounds();

    auto absCentreX = getWidth() / 3;
    auto absCentreY = getHeight() / 3;

    for (auto* b : boxes)
    {
        int offset = first ? 0 : -5;
        b->box.setBounds(x - offset, absCentreY, 70, 20);
        x = b->box.getRight();
        first = false;
    }
}
//==============================================================================

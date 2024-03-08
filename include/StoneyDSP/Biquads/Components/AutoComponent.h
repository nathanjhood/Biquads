/**
 * @file AutoComponent.h
 * @author StoneyDSP (nathanjhood@googlemail.com)
 * @brief
 * @version 0.1
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#define __STONEYDSP_BIQUADS_COMPONENTS_AUTOCOMPONENT_H__

#include <JuceHeader.h>

class AutoComponentLookAndFeel : public juce::LookAndFeel_V4
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

    AutoComponentLookAndFeel();
};

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

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

class AutoComponent : public juce::Component
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    using Lambda = const std::function<void()>;

    AutoComponent(juce::AudioProcessor& p, APVTS& apvts, Lambda& paramLambda = {});

    void paint (juce::Graphics& g) override;
    void resized() override;

private:

    Lambda& lambda;
    AutoComponentLookAndFeel lookAndfeel;
    juce::OwnedArray<SliderWithAttachment> sliders;
    juce::OwnedArray<BoxWithAttachment> boxes;
    juce::OwnedArray<ButtonWithAttachment> buttons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoComponent)
};

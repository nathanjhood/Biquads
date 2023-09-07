/**
 * @file AutoKnob.h
 * @author StoneyDSP (nathanjhood@googlemail.com)
 * @brief
 * @version 0.1
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#define __STONEYDSP_BIQUADS_COMPONENTS_AUTOKNOB_H__


#include <JuceHeader.h>


class AutoKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

    AutoKnobLookAndFeel();
};

using KnobAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

struct KnobWithAttachment
{
    juce::Slider knob;
    std::unique_ptr<KnobAttachment> attachment;
};



class AutoKnob : public juce::Component
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    using Lambda = const std::function<void()>;

    AutoKnob(juce::AudioProcessor& p, APVTS& apvts, Lambda& paramLambda = {});
    ~AutoKnob() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    Lambda& lambdaSupplier;

    AutoKnobLookAndFeel lookAndfeel;
    juce::OwnedArray<KnobWithAttachment> knobs;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutoKnob)
};

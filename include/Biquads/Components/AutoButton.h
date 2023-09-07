/**
 * @file AutoButton.h
 * @author StoneyDSP (nathanjhood@googlemail.com)
 * @brief
 * @version 0.1
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#define __STONEYDSP_BIQUADS_COMPONENTS_AUTOBUTTON_H__

#include <JuceHeader.h>


class AutoButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

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

    AutoButton(juce::AudioProcessor& p, APVTS& apvts, Lambda& paramLambda = {});
    ~AutoButton() override;


    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    Lambda& lambdaSupplier;

    AutoButtonLookAndFeel lookAndfeel;
    juce::OwnedArray<ButtonWithAttachment> buttons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoButton)
};

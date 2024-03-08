/**
 * @file AutoComboBox.h
 * @author StoneyDSP (nathanjhood@googlemail.com)
 * @brief
 * @version 0.1
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#define __STONEYDSP_BIQUADS_COMPONENTS_AUTOCOMBOBOX_H__

#include <JuceHeader.h>

class AutoComboBoxLookAndFeel : public juce::LookAndFeel_V4
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

    AutoComboBoxLookAndFeel();
};

using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

struct BoxWithAttachment
{
    juce::ComboBox box;
    std::unique_ptr<ComboBoxAttachment> attachment;
};

class AutoComboBox : public juce::Component
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

    AutoComboBox(juce::AudioProcessor& p, APVTS& apvts, std::function<void()> paramLambda = {});
    ~AutoComboBox() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:

    AutoComboBoxLookAndFeel lookAndfeel;
    juce::OwnedArray<BoxWithAttachment> boxes;

    std::function<void()> lambdaSupplier;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutoComboBox)
};

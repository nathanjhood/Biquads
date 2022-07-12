/*
  ==============================================================================

    Viewer.h
    Created: 11 Jul 2022 5:25:54am
    Author:  natha

  ==============================================================================
*/

#pragma once

#ifndef VIEWER_H_INCLUDED
#define VIEWER_H_INCLUDED

#include <JuceHeader.h>

#include "../Modules/SecondOrderBiquads.h"

class Viewer : public juce::Component, private juce::Timer
{
public:
    Viewer(juce::AudioProcessorValueTreeState& apvts, juce::dsp::ProcessSpec& spec);
    ~Viewer();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setNeedsCurveUpdate(bool needsUpdate) { needsCurveUpdate.exchange(needsUpdate); }
    void updateCurve();

private:
    void timerCallback() override;
    juce::Path curvePath;
    std::atomic_bool needsCurveUpdate;

    Biquads<float> processor;
    juce::AudioProcessorValueTreeState& apvts;
    juce::dsp::ProcessSpec& setup;

    void processBuffer();
    juce::AudioBuffer<float> dryBuffer;
    juce::AudioBuffer<float> wetBuffer;

    float getMagnitudeForX(float freq);
    juce::dsp::FFT forwardFFT;

    std::unique_ptr<float[]> dryFFTBuffer;
    std::unique_ptr<float[]> wetFFTBuffer;
    std::unique_ptr<float[]> H;

    std::atomic<float>* eqFreqParameter;
    std::atomic<float>* eqQParameter;
    std::atomic<float>* eqGainParameter;
    std::atomic<float>* transParameter;
    std::atomic<float>* typeParameter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Viewer)
};

#endif //VIEWER_H_INCLUDED

/***************************************************************************//**
 * @file Wrapper.cpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief Simple two-pole equalizer with variable oversampling.
 * @version 1.2.1.138
 * @date 2024-03-13
 *
 * @copyright Copyright (c) 2024 - Nathan J. Hood

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

 ******************************************************************************/

#include "StoneyDSP/Biquads.hpp"

namespace StoneyDSP {
/** @addtogroup StoneyDSP @{ */

namespace Biquads {
/** @addtogroup Biquads @{ */

template <typename SampleType>
AudioPluginAudioProcessorWrapper<SampleType>::AudioPluginAudioProcessorWrapper(AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts, juce::dsp::ProcessSpec& spec)
: audioProcessor (p)
, state(apvts)
, setup(spec)
// , parameters(p)
// , parameters(audioProcessor, state)
, mixer()
// , biquad()
, b0            (one)
, b1            (zero)
, b2            (zero)
, a0            (one)
, a1            (zero)
, a2            (zero)
, b_0           (one)
, b_1           (zero)
, b_2           (zero)
, a_0           (one)
, a_1           (zero)
, a_2           (zero)
, loop          (zero)
, outputSample  (zero)
, minFreq       (static_cast <SampleType>(20.0))
, maxFreq       (static_cast <SampleType>(20000.0))
, hz            (static_cast <SampleType>(1000.0))
, q             (static_cast <SampleType>(0.5))
, g             (zero)
, frequencyPtr  (dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("Band_A_frequencyID")))
, resonancePtr  (dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("Band_A_resonanceID")))
, gainPtr       (dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("Band_A_gainID")))
, typePtr       (dynamic_cast <juce::AudioParameterChoice*> (p.getAPVTS().getParameter("Band_A_typeID")))
, transformPtr  (dynamic_cast <juce::AudioParameterChoice*> (p.getAPVTS().getParameter("Master_transformID")))
// , osPtr         (dynamic_cast <juce::AudioParameterChoice*> (p.getAPVTS().getParameter("Master_osID")))
, outputPtr     (dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("Master_outputID")))
, mixPtr        (dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("Master_mixID")))
{

    jassert(frequencyPtr != nullptr);
    jassert(resonancePtr != nullptr);
    jassert(gainPtr != nullptr);
    jassert(typePtr != nullptr);
    jassert(transformPtr != nullptr);
    // jassert(osPtr != nullptr);
    jassert(outputPtr != nullptr);
    jassert(mixPtr != nullptr);

    // auto osFilter = juce::dsp::Oversampling<SampleType>::filterHalfBandFIREquiripple;

    // for (int i = 0; i < 5; ++i)
    //     oversampler[i] = std::make_unique<juce::dsp::Oversampling<SampleType>>
    //     (audioProcessor.getTotalNumInputChannels(), i, osFilter, true, false);

    reset(zero);
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::setFrequency (SampleType newFreq)
{
    jassert(minFreq <= newFreq && newFreq <= maxFreq);

    // if (hz != newFreq)
    // {
        hz = juce::jlimit(minFreq, maxFreq, newFreq);

        omega = (hz * ((pi * two) / static_cast <SampleType>(sampleRate)));
        cos = (std::cos(omega));
        sin = (std::sin(omega));

        calculateCoefficients();
    // }
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::setResonance(SampleType newRes)
{
    jassert(zero <= newRes && newRes <= one);

    // if (q != newRes)
    // {
        q = juce::jlimit(zero, one, newRes);
        calculateCoefficients();
    // }
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::setGain(SampleType newGain)
{
    // if (g != newGain)
    // {
        g = newGain;
        calculateCoefficients();
    // }
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::setFilterType(filterType newFiltType)
{
    if (filterTypeParamValue != newFiltType)
    {
        filterTypeParamValue = newFiltType;

        reset(zero);
        calculateCoefficients();
    }
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::setTransformType(transformationType newTransformType)
{
    if (transformationParamValue != newTransformType)
    {
        transformationParamValue = newTransformType;
        reset(zero);
    }
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    sampleRate = spec.sampleRate;

    // oversamplingFactor = 1 << curOS;
    // prevOS = curOS;

    // for (int i = 0; i < 5; ++i)
    //     oversampler[i]->initProcessing(spec.maximumBlockSize);

    // for (int i = 0; i < 5; ++i)
    //     oversampler[i]->numChannels = (size_t)spec.numChannels;

    Wn_1.resize(spec.numChannels);
    Wn_2.resize(spec.numChannels);
    Xn_1.resize(spec.numChannels);
    Xn_2.resize(spec.numChannels);
    Yn_1.resize(spec.numChannels);
    Yn_2.resize(spec.numChannels);

    minFreq = static_cast <SampleType> (sampleRate / 24576.0);
    maxFreq = static_cast <SampleType> (sampleRate / 2.125);

    jassert(static_cast <SampleType> (20.0) >= minFreq && minFreq <= static_cast <SampleType> (20000.0));
    jassert(static_cast <SampleType> (20.0) <= maxFreq && maxFreq >= static_cast <SampleType> (20000.0));


    reset(zero);

    mixer.prepare(spec);
    // biquad.prepare(spec);
    setFrequency(hz);
    setResonance(q);
    setGain(g);
    setFilterType(filterTypeParamValue);
    setTransformType(transformationParamValue);

    update();
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::reset()
{
    mixer.reset();
    // biquad.reset();

    SampleType initialValue = zero;

    for (auto v : { &Wn_1, &Wn_2, &Xn_1, &Xn_2, &Yn_1, &Yn_2 })
        std::fill(v->begin(), v->end(), initialValue);

    // for (int i = 0; i < 5; ++i)
    //     oversampler[i]->reset();
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::reset(SampleType initialValue)
{
    mixer.reset();

    for (auto v : { &Wn_1, &Wn_2, &Xn_1, &Xn_2, &Yn_1, &Yn_2 })
        std::fill(v->begin(), v->end(), initialValue);

    // for (int i = 0; i < 5; ++i)
    //     oversampler[i]->reset();
}

//==============================================================================
template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    auto totalNumInputChannels  = audioProcessor.getTotalNumInputChannels();
    auto totalNumOutputChannels = audioProcessor.getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        juce::ignoreUnused (channelData);
        // ..do something to the data... (mixer push wet samples)?
    }

    // setOversampling();
    update();

    processBlock(buffer, midiMessages);
    return;
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::processBlock(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::dsp::AudioBlock<SampleType> dryBlock(buffer);
    juce::dsp::AudioBlock<SampleType> wetBlock(buffer);

    mixer.pushDrySamples(dryBlock);

    // wetBlock = oversampler[curOS]->processSamplesUp(dryBlock);

    // This context is intended for use in situations where two different blocks
    // are being used as the input and output to the process algorithm, so the
    // processor must read from the block returned by getInputBlock() and write
    // its results to the block returned by getOutputBlock().
    auto context = juce::dsp::ProcessContextReplacing<SampleType> (wetBlock);

    // biquad.process(context);
    processContext(context);

    // oversampler[curOS]->processSamplesDown(wetBlock);

    for (auto channel = 0; channel < audioProcessor.getTotalNumOutputChannels(); ++channel)
        buffer.applyGain (channel, 0, buffer.getNumSamples(), static_cast<SampleType>(juce::Decibels::decibelsToGain(static_cast<SampleType>(outputPtr->get()), static_cast<SampleType>(-120.00))));

    mixer.mixWetSamples(wetBlock);
    return;
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::processBypass(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::dsp::AudioBlock<SampleType> block(buffer);

    // This context is intended for use in situations where a single block is
    // being used for both the input and output to the process algorithm, so it
    // will return the same object for both its getInputBlock() and
    // getOutputBlock() methods.
    auto context = juce::dsp::ProcessContextReplacing<SampleType>(block);

    const auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();
    const auto numChannels = outputBlock.getNumChannels();
    const auto numSamples = outputBlock.getNumSamples();
    //const auto len = inputBlock.getNumSamples();

    jassert(inputBlock.getNumChannels() == numChannels);
    jassert(inputBlock.getNumSamples() == numSamples);

    outputBlock.copyFrom(inputBlock);
    return;
}

template <typename SampleType>
SampleType AudioPluginAudioProcessorWrapper<SampleType>::processSample(int channel, SampleType inputValue)
{
    // return biquad.processSample(channel, inputValue);

    jassert(juce::isPositiveAndBelow(channel, Wn_1.size()));
    jassert(juce::isPositiveAndBelow(channel, Wn_2.size()));
    jassert(juce::isPositiveAndBelow(channel, Xn_1.size()));
    jassert(juce::isPositiveAndBelow(channel, Xn_2.size()));
    jassert(juce::isPositiveAndBelow(channel, Yn_1.size()));
    jassert(juce::isPositiveAndBelow(channel, Yn_1.size()));

    switch (transformationParamValue)
    {
    case StoneyDSP::Audio::TransformationType::directFormI:
        inputValue = directFormI(channel, inputValue);
        break;
    case StoneyDSP::Audio::TransformationType::directFormII:
        inputValue = directFormII(channel, inputValue);
        break;
    case StoneyDSP::Audio::TransformationType::directFormItransposed:
        inputValue = directFormITransposed(channel, inputValue);
        break;
    case StoneyDSP::Audio::TransformationType::directFormIItransposed:
        inputValue = directFormIITransposed(channel, inputValue);
        break;
    default:
        inputValue = directFormIITransposed(channel, inputValue);
    }

    return inputValue;
}

template <typename SampleType>
SampleType AudioPluginAudioProcessorWrapper<SampleType>::directFormI(int channel, SampleType inputSample)
{
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];
    auto& Yn1 = Yn_1[(size_t)channel];
    auto& Yn2 = Yn_2[(size_t)channel];

    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Yn = ((Xn * b0) + (Xn1 * b1) + (Xn2 * b2) + (Yn1 * a1) + (Yn2 * a2));

    Xn2 = Xn1, Yn2 = Yn1;
    Xn1 = Xn, Yn1 = Yn;

    return Yn;
}

template <typename SampleType>
SampleType AudioPluginAudioProcessorWrapper<SampleType>::directFormII(int channel, SampleType inputSample)
{
    auto& Wn1 = Wn_1[(size_t)channel];
    auto& Wn2 = Wn_2[(size_t)channel];

    auto& Wn = loop;
    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Wn = ( Xn + ((Wn1 * a1) + (Wn2 * a2)));
    Yn = ((Wn * b0) + (Wn1 * b1) + (Wn2 * b2));

    Wn2 = Wn1;
    Wn1 = Wn;

    return Yn;
}

template <typename SampleType>
SampleType AudioPluginAudioProcessorWrapper<SampleType>::directFormITransposed(int channel, SampleType inputSample)
{
    auto& Wn1 = Wn_1[(size_t)channel];
    auto& Wn2 = Wn_2[(size_t)channel];
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];

    auto& Wn = loop;
    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Wn = ( Xn + Wn2);
    Yn = ((Wn * b0) + Xn2);

    Xn2 = ((Wn * b1) + Xn1), Wn2 = ((Wn * a1) + Wn1);
    Xn1 = ( Wn * b2),        Wn1 = ( Wn * a2);

    return Yn;
}

template <typename SampleType>
SampleType AudioPluginAudioProcessorWrapper<SampleType>::directFormIITransposed(int channel, SampleType inputSample)
{
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];

    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Yn  = ((Xn * b0) + (Xn2));

    Xn2 = ((Xn * b1) + (Xn1) + (Yn * a1));
    Xn1 = ((Xn * b2) +         (Yn * a2));

    return Yn;
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::calculateCoefficients()
{
    alpha = (sin * (one - q));
    a = (std::pow(SampleType(10), (g * SampleType(0.05))));
    sqrtA = ((std::sqrt(a) * two) * alpha);

    switch (filterTypeParamValue)
    {
    case filterType::lowPass2:

        b_0 = ((one - cos) / two);
        b_1 = (one - cos);
        b_2 = ((one - cos) / two);
        a_0 = (one + alpha);
        a_1 = (minusTwo * cos);
        a_2 = (one - alpha);

        break;


    case filterType::lowPass1:

        b_0 = (omega / (one + omega));
        b_1 = (omega / (one + omega));
        b_2 = (zero);
        a_0 = (one);
        a_1 = (minusOne * ((one - omega) / (one + omega)));
        a_2 = (zero);

        break;


    case filterType::highPass2:

        b_0 = (one + cos) / two;
        b_1 = (minusOne * (one + cos));
        b_2 = ((one + cos) / two);
        a_0 = (one + alpha);
        a_1 = (minusTwo * cos);
        a_2 = (one - alpha);

        break;


    case filterType::highPass1:

        b_0 = (one / (one + omega));
        b_1 = ((one / (one + omega)) * minusOne);
        b_2 = (zero);
        a_0 = (one);
        a_1 = (((one - omega) / (one + omega)) * minusOne);
        a_2 = (zero);

        break;


    case filterType::bandPass:

        b_0 = (sin / two);
        b_1 = (zero);
        b_2 = (minusOne * (sin / two));
        a_0 = (one + alpha);
        a_1 = (minusTwo * cos);
        a_2 = (one - alpha);

        break;


    case filterType::bandPassQ:

        b_0 = (alpha);
        b_1 = (zero);
        b_2 = (minusOne * alpha);
        a_0 = (one + alpha);
        a_1 = (minusTwo * cos);
        a_2 = (one - alpha);

        break;


    case filterType::lowShelf2:

        b_0 = ((((a + one) - ((a - one) * cos)) + sqrtA) * a);
        b_1 = ((((a - one) - ((a + one) * cos)) * two) * a);
        b_2 = ((((a + one) - ((a - one) * cos)) - sqrtA) * a);
        a_0 = (((a + one) + ((a - one) * cos)) + sqrtA);
        a_1 = (((a - one) + ((a + one) * cos)) * minusTwo);
        a_2 = (((a + one) + ((a - one) * cos)) - sqrtA);

        break;


    case filterType::lowShelf1:

        b_0 = (one + ((omega / (one + omega)) * (minusOne + (a * a))));
        b_1 = ((((omega / (one + omega)) * (minusOne + (a * a))) - ((one - omega) / (one + omega))));
        b_2 = (zero);
        a_0 = (one);
        a_1 = (minusOne * ((one - omega) / (one + omega)));
        a_2 = (zero);

        break;


    case filterType::lowShelf1C:

        b_0 = (one + ((omega / a) / (one + (omega / a)) * (minusOne + (a * a))));
        b_1 = (((((omega / a) / (one + (omega / a))) * (minusOne + (a * a))) - ((one - (omega / a)) / (one + (omega / a)))));
        b_2 = (zero);
        a_0 = (one);
        a_1 = (minusOne * ((one - (omega / a)) / (one + (omega / a))));
        a_2 = (zero);

        break;


    case filterType::highShelf2:

        b_0 = ((((a + one) + ((a - one) * cos)) + sqrtA) * a);
        b_1 = ((((a - one) + ((a + one) * cos)) * minusTwo) * a);
        b_2 = ((((a + one) + ((a - one) * cos)) - sqrtA) * a);
        a_0 = (((a + one) - ((a - one) * cos)) + sqrtA);
        a_1 = (((a - one) - ((a + one) * cos)) * two);
        a_2 = (((a + one) - ((a - one) * cos)) - sqrtA);

        break;


    case filterType::highShelf1:

        b_0 = (one + ((minusOne + (a * a)) / (one + omega)));
        b_1 = (minusOne * (((one - omega) / (one + omega)) + ((minusOne + (a * a)) / (one + omega))));
        b_2 = (zero);
        a_0 = (one);
        a_1 = (minusOne * ((one - omega) / (one + omega)));
        a_2 = (zero);

        break;


    case filterType::highShelf1C:

        b_0 = (one + ((minusOne + (a * a)) / (one + (omega * a))));
        b_1 = (minusOne * (((one - (omega * a)) / (one + (omega * a))) + ((minusOne + (a * a)) / (one + (omega * a)))));
        b_2 = (zero);
        a_0 = (one);
        a_1 = (minusOne * ((one - (omega * a)) / (one + (omega * a))));
        a_2 = (zero);

        break;


    case filterType::peak:

        b_0 = (one + (alpha * a));
        b_1 = (minusTwo * cos);
        b_2 = (one - (alpha * a));
        a_0 = (one + (alpha / a));
        a_1 = (minusTwo * cos);
        a_2 = (one - (alpha / a));

        break;


    case filterType::notch:

        b_0 = (one);
        b_1 = (minusTwo * cos);
        b_2 = (one);
        a_0 = (one + alpha);
        a_1 = (minusTwo * cos);
        a_2 = (one - alpha);

        break;


    case filterType::allPass:

        b_0 = (one - alpha);
        b_1 = (minusTwo * cos);
        b_2 = (one + alpha);
        a_0 = (one + alpha);
        a_1 = (minusTwo * cos);
        a_2 = (one - alpha);

        break;


    default:

        b_0 = one;
        b_1 = zero;
        b_2 = zero;
        a_0 = one;
        a_1 = zero;
        a_2 = zero;

        break;
    }

    a0 = (  one  / a_0);
    a1 = ((-a_1) * a0);
    a2 = ((-a_2) * a0);
    b0 = (  b_0  * a0);
    b1 = (  b_1  * a0);
    b2 = (  b_2  * a0);
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::snapToZero() noexcept
{
    for (auto v : { &Wn_1, &Wn_2, &Xn_1, &Xn_2, &Yn_1, &Yn_2 })
        for (auto& element : *v)
            juce::dsp::util::snapToZero(element);
}

template <typename SampleType>
void AudioPluginAudioProcessorWrapper<SampleType>::update()
{
    mixer.setWetMixProportion(static_cast   <SampleType> (mixPtr->get() * 0.01));
    setFrequency     (static_cast   <SampleType>    (frequencyPtr->get()));
    setResonance     (static_cast   <SampleType>    (resonancePtr->get()));
    setGain          (static_cast   <SampleType>    (gainPtr->get()));
    setFilterType    (static_cast   <StoneyDSP::Audio::FilterType>          (typePtr->getIndex()));
    setTransformType (static_cast   <StoneyDSP::Audio::TransformationType>  (transformPtr->getIndex()));
}

// template <typename SampleType>
// void AudioPluginAudioProcessorWrapper<SampleType>::setOversampling()
// {
//     curOS = (int)osPtr->getIndex();
//     if (curOS != prevOS)
//     {
//         oversamplingFactor = 1 << curOS;
//         prevOS = curOS;
//         mixer.reset();
//         // biquad.reset();
//         // output.reset();
//     }
// }

// template <typename SampleType>
// SampleType AudioPluginAudioProcessorWrapper<SampleType>::getLatencySamples() const noexcept
// {
//     return oversampler[curOS]->getLatencyInSamples();
// }
//==============================================================================
template class AudioPluginAudioProcessorWrapper<float>;
template class AudioPluginAudioProcessorWrapper<double>;

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP

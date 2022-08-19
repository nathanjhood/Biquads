/*
  ==============================================================================

    Transforms.cpp
    Created: 15 Jul 2022 1:55:41am
    Author:  natha

  ==============================================================================
*/

#include "Transforms.h"

template <typename SampleType>
IIRTransforms<SampleType>::IIRTransforms()
{

}

template <typename SampleType>
IIRTransforms<SampleType>::~IIRTransforms()
{

}

template <typename SampleType>
void IIRTransforms<SampleType>::setTransformType(TransformationType newTransformType)
{
    if (transformType != newTransformType)
    {
        transformType = newTransformType;
        reset();
    }
}

template <typename SampleType>
void IIRTransforms<SampleType>::prepare(juce::dsp::ProcessSpec spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    sampleRate = spec.sampleRate;

    Wn_1.resize(spec.numChannels);
    Wn_2.resize(spec.numChannels);
    Xn_1.resize(spec.numChannels);
    Xn_2.resize(spec.numChannels);
    Yn_1.resize(spec.numChannels);
    Yn_2.resize(spec.numChannels);
}

template <typename SampleType>
void IIRTransforms<SampleType>::reset(SampleType initValue)
{
    for (auto v : { &Wn_1, &Wn_2, &Xn_1, &Xn_2, &Yn_1, &Yn_2 })
        std::fill(v->begin(), v->end(), initValue);
}

//template <typename SampleType>
//SampleType IIRTransforms<SampleType>::processSample(int channel, SampleType inputValue)
//{
//    jassert(juce::isPositiveAndBelow(channel, Wn_1.size()));
//    jassert(juce::isPositiveAndBelow(channel, Wn_2.size()));
//    jassert(juce::isPositiveAndBelow(channel, Xn_1.size()));
//    jassert(juce::isPositiveAndBelow(channel, Xn_2.size()));
//    jassert(juce::isPositiveAndBelow(channel, Yn_1.size()));
//    jassert(juce::isPositiveAndBelow(channel, Yn_1.size()));
//
//
//    switch (transformType)
//    {
//    case TransformationType::directFormI:
//        inputValue = directFormI(channel, inputValue);
//        break;
//    case TransformationType::directFormII:
//        inputValue = directFormII(channel, inputValue);
//        break;
//    case TransformationType::directFormItransposed:
//        inputValue = directFormITransposed(channel, inputValue);
//        break;
//    case TransformationType::directFormIItransposed:
//        inputValue = directFormIITransposed(channel, inputValue);
//        break;
//    default:
//        inputValue = directFormIITransposed(channel, inputValue);
//    }
//
//    return inputValue;
//}

template <typename SampleType>
SampleType IIRTransforms<SampleType>::directFormI(int channel, SampleType& inputSample)
{
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];
    auto& Yn1 = Yn_1[(size_t)channel];
    auto& Yn2 = Yn_2[(size_t)channel];

    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Yn = ((Xn *= b[0]) + (Xn1 *= b[1]) + (Xn2 *= b[2]) + (Yn1 *= a[1]) + (Yn2 *= a[2]));

    Xn2 = Xn1;
    Yn2 = Yn1;
    Xn1 = Xn;
    Yn1 = Yn;

    return Yn;
}

template <typename SampleType>
SampleType IIRTransforms<SampleType>::directFormII(int channel, SampleType& inputSample)
{
    auto& Wn1 = Wn_1[(size_t)channel];
    auto& Wn2 = Wn_2[(size_t)channel];

    auto& Wn = loop;
    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Wn = (Xn + ((Wn1 *= a[1]) + (Wn2 *= a[2])));
    Yn = ((Wn *= b[0]) + (Wn1 *= b[1]) + (Wn2 *= b[2]));

    Wn2 = Wn1;
    Wn1 = Wn;

    return Yn;
}

template <typename SampleType>
SampleType IIRTransforms<SampleType>::directFormITransposed(int channel, SampleType& inputSample)
{
    auto& Wn1 = Wn_1[(size_t)channel];
    auto& Wn2 = Wn_2[(size_t)channel];
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];

    auto& Wn = loop;
    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Wn = (Xn + Wn2);
    Yn = ((Wn *= b[0]) + Xn2);

    Xn2 = ((Wn *= b[1]) + Xn1);
    Wn2 = ((Wn *= a[1]) + Wn1);
    Xn1 = (Wn *= b[2]);
    Wn1 = (Wn *= a[2]);

    return Yn;
}

template <typename SampleType>
SampleType IIRTransforms<SampleType>::directFormIITransposed(int channel, SampleType& inputSample)
{
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];

    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Yn = ((Xn *= b[0]) + (Xn2));

    Xn2 = ((Xn *= b[1]) + (Xn1) + (Yn *= a[1]));
    Xn1 = ((Xn *= b[2]) + (Yn *= a[2]));

    return Yn;
}

template <typename SampleType>
void IIRTransforms<SampleType>::snapToZero() noexcept
{
    for (auto v : { &Wn_1, &Wn_2, &Xn_1, &Xn_2, &Yn_1, &Yn_2 })
        for (auto& element : *v)
            juce::dsp::util::snapToZero(element);
}

//==============================================================================
template class IIRTransforms<float>;
template class IIRTransforms<double>;

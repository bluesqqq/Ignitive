#pragma once

#include <JuceHeader.h>

#include "DistortionProcessor.h"
#include "FeedbackProcessor.h"
#include "FilterProcessor.h"

#include "EnvelopeFollower.h"

#include "ModMatrix.h"

class IgnitiveEngine : public juce::dsp::ProcessorBase {
    private:
        juce::AudioProcessorValueTreeState& parameters;
        juce::AudioProcessor& processor;

        //==============// Gain //==============//
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> inGain;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> outGain;

    public:
        //==============// DSP //==============//
        DistortionProcessor distortion;
        FeedbackProcessor feedback;
        FilterProcessor filter;

        EnvelopeFollower envelope;

        ModMatrix modMatrix;

        IgnitiveEngine(juce::AudioProcessorValueTreeState& parameters, juce::AudioProcessor& processor);

        void prepare(const juce::dsp::ProcessSpec& spec) override;

        void process(const juce::dsp::ProcessContextReplacing<float>& context) override;

        void reset() override;
};
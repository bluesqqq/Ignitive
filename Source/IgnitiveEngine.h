#pragma once

#include <JuceHeader.h>

#include "GainProcessor.h"
#include "DistortionProcessor.h"
#include "FeedbackProcessor.h"
#include "FilterProcessor.h"

#include "EnvelopeFollower.h"
#include "LFO.h"

#include "ModMatrix.h"

class IgnitiveEngine : public juce::dsp::ProcessorBase {
    private:
        juce::AudioProcessorValueTreeState& parameters;
        juce::AudioProcessor& processor;

    public:
        //==============// DSP //==============//
        GainProcessor inGain, outGain;
        DistortionProcessor distortion;
        FeedbackProcessor feedback;
        FilterProcessor filter;

        EnvelopeFollower envelope;
        LFO lfo;

        ModMatrix modMatrix;

        IgnitiveEngine(juce::AudioProcessorValueTreeState& parameters, juce::AudioProcessor& processor);

        void prepare(const juce::dsp::ProcessSpec& spec) override;

        void process(const juce::dsp::ProcessContextReplacing<float>& context) override;

        void reset() override;
};
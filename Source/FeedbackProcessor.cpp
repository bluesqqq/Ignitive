#include "FeedbackProcessor.h"
#include "Parameters.h"

FeedbackProcessor::FeedbackProcessor(juce::AudioProcessorValueTreeState& params) : parameters(params), sampleRate(48000) {

}

void FeedbackProcessor::prepare(const juce::dsp::ProcessSpec& spec) {
    sampleRate = spec.sampleRate;

    delayLines.clear();
    delayLines.reserve(spec.numChannels);

    for (int channels = 0; channels < spec.numChannels; ++channels) {
        auto delayLine = std::make_unique<juce::dsp::DelayLine<float>>(2.0 * spec.sampleRate); // 2s buffer
        delayLine->prepare(spec);
        delayLines.push_back(std::move(delayLine));
    }
}

void FeedbackProcessor::process(const juce::dsp::ProcessContextReplacing<float>& context) {
    auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();

    const auto numChannels = inputBlock.getNumChannels();
    const auto numSamples = inputBlock.getNumSamples();

    float feedback = parameters.getRawParameterValue(Parameters::ID_FEEDBACK)->load();
    float delay = parameters.getRawParameterValue(Parameters::ID_FEEDBACK_DELAY)->load();
    auto delaySamples = delay * sampleRate;

    for (size_t channel = 0; channel < numChannels; ++channel) {
        auto* in = inputBlock.getChannelPointer(channel);
        auto* out = outputBlock.getChannelPointer(channel);

        auto& delayLine = *delayLines[channel];
        delayLine.setDelay(delaySamples);

        for (size_t n = 0; n < numSamples; ++n) {
            float inputSample = in[n];
            float delayedSample = delayLine.popSample(0);

            float toDelay = inputSample + delayedSample * feedback;

            delayLine.pushSample(0, toDelay);

            out[n] = delayedSample;
        }
    }
}

void FeedbackProcessor::reset() {
}

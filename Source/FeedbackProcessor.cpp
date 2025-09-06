#include "FeedbackProcessor.h"
#include "Parameters.h"

FeedbackProcessor::FeedbackProcessor(juce::AudioProcessorValueTreeState& params, const juce::String& amountID, const juce::String& delayID)
    : parameters(params), sampleRate(48000), amountID(amountID), delayID(delayID) {

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
    auto& block = context.getOutputBlock();

    const auto numChannels = block.getNumChannels();
    const auto numSamples = block.getNumSamples();

    updateParameters();

    for (size_t sample = 0; sample < numSamples; ++sample) {
        float fb = amount.getNextValue();
        float dSec = delay.getNextValue();
        float delaySamples = dSec * (float)sampleRate;

        for (size_t channel = 0; channel < numChannels; ++channel) {
            auto& delayLine = *delayLines[channel];
            delayLine.setDelay(delaySamples);

            float* data = block.getChannelPointer(channel);
            data[sample] += delayLine.popSample(channel) * fb;
        }
    }
}

void FeedbackProcessor::reset() {
}

void FeedbackProcessor::processWrite(const juce::dsp::AudioBlock<float>& block) {
    const auto numChannels = block.getNumChannels();
    const auto numSamples = block.getNumSamples();

    for (size_t channel = 0; channel < numChannels; ++channel) {
        auto& delayLine = *delayLines[channel];
        const float* data = block.getChannelPointer(channel);

        for (size_t sample = 0; sample < numSamples; ++sample) {
            delayLine.pushSample(channel, data[sample]);
        }
    }
}

float FeedbackProcessor::processSample(float input, int channel) {
    if (channel >= delayLines.size()) return input;

    auto& delayLine = *delayLines[channel];

    float fb = amount.getNextValue();
    float dSec = delay.getNextValue();
    float delaySamples = dSec * (float)sampleRate;

    delayLine.setDelay(delaySamples);

    float delayed = delayLine.popSample(channel);
    return input + delayed * fb;
}

void FeedbackProcessor::processWriteSample(float input, int channel) {
    if (channel >= delayLines.size()) return;

    auto& delayLine = *delayLines[channel];
    delayLine.pushSample(channel, input);
}

void FeedbackProcessor::updateParameters() {
    amount.setTargetValue(*parameters.getRawParameterValue(Parameters::ID_FEEDBACK));
    delay.setTargetValue(*parameters.getRawParameterValue(Parameters::ID_FEEDBACK_DELAY));
}

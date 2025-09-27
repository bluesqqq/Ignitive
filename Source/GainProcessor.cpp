#include "GainProcessor.h"

GainProcessor::GainProcessor(juce::AudioProcessorValueTreeState& params, const juce::String& gainID) : parameters(params), gainID(gainID) {
}

void GainProcessor::prepare(const juce::dsp::ProcessSpec& spec) {
    auto secondsPerBlock = spec.maximumBlockSize / spec.sampleRate;
	gain.reset(spec.sampleRate, 0.05);
}

void GainProcessor::process(const juce::dsp::ProcessContextReplacing<float>& context) {
	float gainValue = parameters.getRawParameterValue(gainID)->load();

    if (std::abs(gainValue - gain.getTargetValue()) > 0.001f) gain.setTargetValue(gainValue);

    auto& block = context.getOutputBlock();

    size_t numSamples = block.getNumSamples();
    size_t numChannels = block.getNumChannels();

    float blockPeak = 0.0f;
    float sumSquares = 0.0f;
    for (size_t sample = 0; sample < numSamples; ++sample) {
        float currentGain = gain.getNextValue();
        for (size_t channel = 0; channel < numChannels; ++channel) {
            float& s = block.getChannelPointer(channel)[sample];
            s *= currentGain;
            sumSquares += s * s;

            // track peak
            float absSample = std::abs(s);
            if (absSample > blockPeak)
                blockPeak = absSample;
        }
    }

    lastBlockPeak = blockPeak;
    lastBlockRMS = std::sqrt(sumSquares / (numSamples * numChannels));
}

void GainProcessor::reset() {
}

float GainProcessor::getRMS() {
    return lastBlockRMS;
}

float GainProcessor::getPeak() {
    return lastBlockPeak;
}

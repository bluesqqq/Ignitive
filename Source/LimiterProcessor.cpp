/*
  ==============================================================================

    LimiterProcessor.cpp
    Created: 26 Sep 2025 10:21:02pm
    Author:  noobs

  ==============================================================================
*/

#include "LimiterProcessor.h"

void LimiterProcessor::prepare(const juce::dsp::ProcessSpec& spec) {
    sampleRate = spec.sampleRate;
    numChannels = static_cast<int>(spec.numChannels);

    gain.assign(numChannels, 1.0f);

    setThreshold(0.99f);
    setReleaseMs(50.0f);
}

void LimiterProcessor::process(const juce::dsp::ProcessContextReplacing<float>& context) {
    auto& block = context.getOutputBlock();
    auto numSamples = block.getNumSamples();

    float releaseCoeff = std::exp(-1.0f / (0.001f * releaseMs * sampleRate));

    for (int channel = 0; channel < numChannels; ++channel) {
        auto* channelData = block.getChannelPointer(channel);

        for (int sample = 0; sample < numSamples; ++sample) {
            float input = channelData[sample];

            float absInput = std::fabsf(input);
            float targetGain = (absInput > threshold) ? (threshold / absInput) : 1.0f;

            if (targetGain < gain[channel]) gain[channel] = targetGain;
            else gain[channel] = gain[channel] * releaseCoeff + (1.0f - releaseCoeff) * targetGain;
            
            channelData[sample] = input * gain[channel];
        }
    }
}

void LimiterProcessor::reset() {
    gain.assign(numChannels, 1.0f);
}

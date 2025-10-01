#include "EnvelopeFollower.h"
#include <cmath>

EnvelopeFollower::EnvelopeFollower(juce::AudioProcessorValueTreeState& params, const juce::String& attackID, const juce::String& releaseID, const juce::String& gateID) : parameters(params), attackID(attackID), releaseID(releaseID), gateID(gateID) {
    fifoBuffer.resize(fifo.getTotalSize());
    writeToFifo(0.0f);
}

void EnvelopeFollower::process(const juce::dsp::AudioBlock<float>& block) {
	setAttackTime(parameters.getRawParameterValue(attackID)->load());
	setReleaseTime(parameters.getRawParameterValue(releaseID)->load());
	setGate(parameters.getRawParameterValue(gateID)->load());

	auto numSamples = (int)block.getNumSamples();
	auto numChannels = (int)block.getNumChannels();

	auto* out = modulationBuffer.getWritePointer(0);

	for (int sample = 0; sample < numSamples; ++sample) {
		float s = 0.0f;

		for (int channel = 0; channel < numChannels; ++channel)
			s += std::abs(block.getSample(channel, sample));

		s /= (float)numChannels;

		out[sample] = processSample(s);

		writeToFifo(out[sample]);
	}
}

float EnvelopeFollower::processSample(float sample) {
    float rmsValue = rmsFollower.processSample(sample);

    float delta = rmsValue - currentValue;

    if (delta > 0)
        currentValue += delta * attackCoef;   // attack
    else
        currentValue += delta * releaseCoef;  // release

    return rmsValue;
}

void EnvelopeFollower::setAttackTime(float attackMs) {
    attackTime = attackMs;
	float attackSamples = attackTime * 0.001f * sampleRate;
	attackCoef = 1.0f - std::exp(std::log(0.001f) / attackSamples);
}

void EnvelopeFollower::setReleaseTime(float releaseMs) {
    releaseTime = releaseMs;
	float releaseSamples = releaseTime * 0.001f * sampleRate;
	releaseCoef = 1.0f - std::exp(std::log(0.001f) / releaseSamples);
}

void EnvelopeFollower::setGate(float g) { gate = g; }
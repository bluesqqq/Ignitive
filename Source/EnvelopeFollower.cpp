#include "EnvelopeFollower.h"
#include <cmath>

EnvelopeFollower::EnvelopeFollower(juce::AudioProcessorValueTreeState& params, const juce::String& attackID, const juce::String& releaseID, const juce::String& gateID)
	: ModSource(juce::Colours::yellow, { 0.0f, 1.0f }), 
	  ParametersDisplayInterface({ "Attack", "Release", "Gate" }),
	  parameters(params), attackID(attackID), releaseID(releaseID), gateID(gateID) {
    fifoBuffer.resize(fifo.getTotalSize());
    writeToFifo(0.0f);

	valueGetters.push_back([this]() { return juce::String(attackTime, 2) + " ms"; });
	valueGetters.push_back([this]() { return juce::String(releaseTime, 2) + " ms"; });
	valueGetters.push_back([this]() { return juce::String(juce::Decibels::gainToDecibels(gate), 2); });
}

void EnvelopeFollower::process(const juce::dsp::AudioBlock<float>& block) {
	setAttackTime(parameters.getRawParameterValue(attackID)->load());
	setReleaseTime(parameters.getRawParameterValue(releaseID)->load());
	setGate(parameters.getRawParameterValue(gateID)->load());

	auto numSamples = (int)block.getNumSamples();
	auto numChannels = (int)block.getNumChannels();

	for (int sample = 0; sample < numSamples; ++sample) {
		float s = 0.0f;

		for (int channel = 0; channel < numChannels; ++channel)
			s += std::abs(block.getSample(channel, sample));

		s /= (float)numChannels;

		modulationBuffer[sample] = processSample(s);

		writeToFifo(modulationBuffer[sample]);
	}
}

float EnvelopeFollower::processSample(float sample) {
    float peakValue = juce::jlimit(0.0f, 1.0f, peakFollower.processSample(sample));

    float delta = peakValue - currentValue;

	if (peakValue >= gate) {
		// Above gate - follow the signal
		if (delta >= 0) {
			// Signal increasing - attack
			currentValue += delta * attackCoef;
		}
		else {
			// Signal decreasing - release
			currentValue += delta * releaseCoef;
		}
	}
	else {
		// Below gate - always release toward zero
		currentValue -= currentValue * releaseCoef;
	}

    return currentValue;
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

void EnvelopeFollower::reset() {
	ModSource::reset();
	peakFollower.reset();
}
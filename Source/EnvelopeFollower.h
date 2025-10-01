#pragma once

#include <vector>
#include "ModSource.h"


class RMSFollower {
	private:
		size_t windowSize = 0;
		float sampleRate = 0;
		float sumSquares = 0;

		std::vector<float> buffer;
		int index = 0;

	public:
		RMSFollower() {}

		void prepare(const juce::dsp::ProcessSpec& spec) {
			sampleRate = spec.sampleRate;
			windowSize = static_cast<size_t>(0.02f * sampleRate); // 20 ms window
			buffer.assign(windowSize, 0.0f);
			sumSquares = 0.0f;
			index = 0;
		}

		/// <summary>
		/// Processes a single sample and returns the RMS value.
		/// </summary>
		float processSample(float sample) {
			float squared = sample * sample;

			sumSquares -= buffer[index];

			sumSquares = juce::jmax(0.0f, sumSquares);

			buffer[index] = squared;
			sumSquares += squared;

			index = (index + 1) % buffer.size();

			float result = std::sqrt(sumSquares / (float)buffer.size());
			jassert(!std::isnan(result));
			return result;
		}

		void reset() {
			std::fill(buffer.begin(), buffer.end(), 0.0f);
			sumSquares = 0.0f;
			index = 0;
		}
};

class EnvelopeFollower : public ModSource {
	private:
		juce::AudioProcessorValueTreeState& parameters;

		float attackTime, releaseTime;
		float gate;
		float attackCoef, releaseCoef;

		float currentValue = 0;
		RMSFollower rmsFollower;

		juce::String attackID;
		juce::String releaseID;
		juce::String gateID;

		float processSample(float sample);

		void setAttackTime(float attackMs);
		void setReleaseTime(float releaseMs);
		void setGate(float g);

	public:
		EnvelopeFollower(juce::AudioProcessorValueTreeState& parameters, const juce::String& attackID, const juce::String& releaseID, const juce::String& gateID);

		void prepare(const juce::dsp::ProcessSpec& spec) override {
			ModSource::prepare(spec);
			rmsFollower.prepare(spec);
		}

		void process(const juce::dsp::AudioBlock<float>& block);

		void reset() override {
			ModSource::reset();
			rmsFollower.reset();
		}
};
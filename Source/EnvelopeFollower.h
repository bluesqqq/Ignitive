#pragma once

#include <vector>
#include "ModSource.h"
#include "ParametersDisplay.h"

class PeakFollower {
private:
    float sampleRate = 0.0f;
    float releaseTime = 0.05f; // seconds
    float releaseCoef = 0.0f;
    float currentPeak = 0.0f;

public:
    PeakFollower() {}

    void prepare(const juce::dsp::ProcessSpec& spec, float releaseSeconds = 0.01f) {
        sampleRate = spec.sampleRate;
        releaseTime = releaseSeconds;
        releaseCoef = std::exp(-1.0f / (releaseTime * sampleRate));
        currentPeak = 0.0f;
    }

    /// <summary>
    /// Processes a single sample and returns the peak value.
    /// </summary>
    float processSample(float sample) {
        float absSample = std::abs(sample);

        if (absSample > currentPeak) {
            currentPeak = absSample; // immediate attack
        }
        else {
            currentPeak *= releaseCoef; // decay
        }

        jassert(!std::isnan(currentPeak));
        return currentPeak;
    }

    void reset() {
        currentPeak = 0.0f;
    }

    void setReleaseTime(float newReleaseTime) {
        releaseTime = newReleaseTime;
        if (sampleRate > 0.0f)
            releaseCoef = std::exp(-1.0f / (releaseTime * sampleRate));
    }
};


class EnvelopeFollower : public ModSource, public ParametersDisplayInterface {
	private:
		juce::AudioProcessorValueTreeState& parameters;

		float attackTime, releaseTime;
		float gate;
		float attackCoef, releaseCoef;

		float currentValue = 0;
		PeakFollower peakFollower;

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
            peakFollower.prepare(spec);
		}

		void process(const juce::dsp::AudioBlock<float>& block);

        void reset() override;
};
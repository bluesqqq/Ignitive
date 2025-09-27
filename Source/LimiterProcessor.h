#pragma once

#include <JuceHeader.h>
#include <vector>
#include <cmath>

class LimiterProcessor : public juce::dsp::ProcessorBase {
	private:
		double sampleRate = 44100.0;
		int numChannels = 2;

		std::vector<float> gain;

		float threshold = 0.99f;
		float releaseMs = 1.0f;

	public:
		LimiterProcessor() = default;

		void prepare(const juce::dsp::ProcessSpec& spec) override;
		void process(const juce::dsp::ProcessContextReplacing<float>& context) override;
		void reset() override;

		void setThreshold(float newThreshold) {
			threshold = juce::jlimit(0.0f, 1.0f, newThreshold);
		}

		void setReleaseMs(float ms) {
			releaseMs = ms;
		}
};
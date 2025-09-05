#pragma once

#include <JuceHeader.h>

class FeedbackProcessor : public juce::dsp::ProcessorBase {
	private:
		juce::AudioProcessorValueTreeState& parameters;

		std::vector<std::unique_ptr<juce::dsp::DelayLine<float>>> delayLines;

		int sampleRate;

	public:
		FeedbackProcessor(juce::AudioProcessorValueTreeState& params);

		void prepare(const juce::dsp::ProcessSpec& spec) override;
		void process(const juce::dsp::ProcessContextReplacing<float>& context) override;
		void reset() override;
};
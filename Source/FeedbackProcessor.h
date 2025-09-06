#pragma once

#include <JuceHeader.h>

class FeedbackProcessor : public juce::dsp::ProcessorBase {
	private:
		juce::AudioProcessorValueTreeState& parameters;

		std::vector<std::unique_ptr<juce::dsp::DelayLine<float>>> delayLines;

		int sampleRate;

		juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> amount;
		juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> delay;

		juce::String amountID;
		juce::String delayID;

	public:
		FeedbackProcessor(juce::AudioProcessorValueTreeState& params, const juce::String& amountID, const juce::String& delayID);

		void prepare(const juce::dsp::ProcessSpec& spec) override;
		void process(const juce::dsp::ProcessContextReplacing<float>& context) override;
		void reset() override;

		void processWrite(const juce::dsp::AudioBlock<float>& block);

		float processSample(float input, int channel);
		void processWriteSample(float input, int channel);

		void updateParameters();

};
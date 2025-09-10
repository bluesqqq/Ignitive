#pragma once

#include <JuceHeader.h>
#include "ModMatrix.h"

class FeedbackProcessor : public juce::dsp::ProcessorBase {
	private:
		juce::AudioProcessorValueTreeState& parameters;
		ModMatrix& modMatrix;

		std::vector<std::unique_ptr<juce::dsp::DelayLine<float>>> delayLines;

		int sampleRate;

		juce::String amountID;
		juce::String delayID;

	public:
		FeedbackProcessor(juce::AudioProcessorValueTreeState& parameters, ModMatrix& modMatrix, const juce::String& amountID, const juce::String& delayID);

		void prepare(const juce::dsp::ProcessSpec& spec) override;
		void process(const juce::dsp::ProcessContextReplacing<float>& context) override;
		void reset() override;

		void processBlockSample(juce::dsp::AudioBlock<float>& block, size_t sample);
		void processWriteBlockSample(juce::dsp::AudioBlock<float>& block, size_t sample);
};
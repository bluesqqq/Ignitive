#pragma once

#include <JuceHeader.h>

class GainProcessor : public juce::dsp::ProcessorBase {
private:
	juce::AudioProcessorValueTreeState& parameters;

	juce::String gainID;

	juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gain;

	float lastBlockRMS = 0.0f;
	float lastBlockPeak = 0.0f;

public:
	GainProcessor(juce::AudioProcessorValueTreeState& parameters, const juce::String& gainID);

	void prepare(const juce::dsp::ProcessSpec& spec) override;
	void process(const juce::dsp::ProcessContextReplacing<float>& context) override;
	void reset() override;

	float getRMS();

	float getPeak();
};
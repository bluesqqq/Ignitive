#pragma once

#include <JuceHeader.h>

class FilterProcessor : public juce::dsp::ProcessorBase {
	private:
		juce::AudioProcessorValueTreeState& parameters;

		juce::dsp::StateVariableTPTFilter<float> filter;

		juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> cutoff;
		juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> resonance;

	public:
		FilterProcessor(juce::AudioProcessorValueTreeState& parameters, const juce::String& cutoffID, const juce::String& resonanceID, const juce::String& typeID, const juce::String& enabledID);

		void prepare(const juce::dsp::ProcessSpec& spec) override;
		void process(const juce::dsp::ProcessContextReplacing<float>& context) override;
		void reset() override;

		void processBlockSample(juce::dsp::AudioBlock<float>& block, size_t sample);

		void updateParameters();

		juce::String cutoffID;
		juce::String resonanceID;
		juce::String typeID;
		juce::String enabledID;
};
#pragma once

#include <JuceHeader.h>
#include "ModMatrix.h"

class FilterProcessor : public juce::dsp::ProcessorBase {
	private:
		juce::AudioProcessorValueTreeState& parameters;
		ModMatrix& modMatrix;

	public:
		FilterProcessor(juce::AudioProcessorValueTreeState& parameters, ModMatrix& modMatrix, const juce::String& lpCutoffID, const juce::String& lpResonanceID, const juce::String& hpCutoffID, const juce::String& hpResonanceID);

		void prepare(const juce::dsp::ProcessSpec& spec) override;
		void process(const juce::dsp::ProcessContextReplacing<float>& context) override;
		void reset() override;

		void processBlockSample(juce::dsp::AudioBlock<float>& block, size_t sample);

		juce::String lpCutoffID;
		juce::String lpResonanceID;
		juce::String hpCutoffID;
		juce::String hpResonanceID;

		juce::dsp::StateVariableTPTFilter<float> lpFilter;
		juce::dsp::StateVariableTPTFilter<float> hpFilter;
};
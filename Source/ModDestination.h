#pragma once

#include <JuceHeader.h>
#include "ModSource.h"

/*
	Represents a modulation destination that can be modulated by one or more sources.

	ModDestination holds a smoothed base value and can apply modulation from ModSources
	(handled by ModMatrix). It manages a buffer of values for per-sample processing.
*/
class ModDestination {
	private:
		// The base value, smoothed, before any modulation
		juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> baseValue;
		// Buffer holding per-sample values after modulation
		std::vector<float> valueBuffer;

	public:
		// GUI display name for this destination
		juce::String displayName;

		ModDestination(const juce::String& displayName = "NO NAME") : displayName(displayName) {}

		void prepare(const juce::dsp::ProcessSpec& spec);
		void process(const juce::dsp::AudioBlock<float>& block);
		void reset();

		void setBaseValue(float value);

		// Returns the current value at the given sample index
		float getValue(int sampleIndex) const;

		// Applies modulation from a given source with specified depth
		void addMod(ModSource* source, float depth);
};
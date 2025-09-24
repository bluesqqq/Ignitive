#pragma once

#include <JuceHeader.h>
#include "ModSource.h"

class ModDestination {
	private:
		juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> baseValue;
		juce::AudioBuffer<float> valueBuffer;

	public:
		juce::String displayName;

		ModDestination(const juce::String& displayName = "NO NAME") : displayName(displayName) {}

		void prepare(const juce::dsp::ProcessSpec& spec);
		void update(float value);

		float getValue(int sampleIndex) const;

		void addMod(ModSource* source, float depth);
};
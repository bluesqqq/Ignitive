#pragma once

#include <JuceHeader.h>
#include "ModSource.h"

class ModDestination {
	private:
		juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> baseValue;
		juce::AudioBuffer<float> valueBuffer;

	public:
		void prepare(const juce::dsp::ProcessSpec& spec);

		void update(float value) {
			baseValue.setTargetValue(value);

			auto* out = valueBuffer.getWritePointer(0);

			// Fills the buffer with the base value
			for (int i = 0; i < valueBuffer.getNumSamples(); i++) {
				out[i] = baseValue.getNextValue();
			}
		}

		float getValue(int sampleIndex) const;

		void addMod(ModSource* source, float depth) {
			if (source == nullptr) return;
			auto* out = valueBuffer.getWritePointer(0);

			for (int i = 0; i < valueBuffer.getNumSamples(); i++) {
				out[i] += source->getValue(i) * depth;
			}
		}
};
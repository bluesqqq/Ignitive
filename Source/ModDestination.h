#pragma once

#include <JuceHeader.h>
#include "ModSource.h"

struct ModDestination {
	juce::RangedAudioParameter* parameter;
	juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> baseValue;
	juce::AudioBuffer<float> valueBuffer;

	ModDestination(juce::RangedAudioParameter* parameter);

	void prepare(const juce::dsp::ProcessSpec& spec) {
		baseValue.reset(spec.sampleRate, 0.02);
		valueBuffer.setSize(1, spec.maximumBlockSize);
	}

	void update() {
		baseValue.setTargetValue(parameter->getValue());

		auto* out = valueBuffer.getWritePointer(0);

		// Fills the buffer with the base value
		for (int i = 0; i < valueBuffer.getNumSamples(); i++) {
			out[i] = baseValue.getNextValue();
		}
	}

	float getValue(int sampleIndex) const {
		return valueBuffer.getSample(0, sampleIndex);
	}

	void addMod(ModSource* source) {
		if (source == nullptr) return;
		auto* out = valueBuffer.getWritePointer(0);

		for (int i = 0; i < valueBuffer.getNumSamples(); i++) {
			out[i] += source->getValue(i);
		}
	}
};
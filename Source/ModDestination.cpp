#include "ModDestination.h"

void ModDestination::prepare(const juce::dsp::ProcessSpec& spec) {
	baseValue.reset(spec.sampleRate, 0.02);
	valueBuffer.setSize(1, spec.maximumBlockSize);
}

void ModDestination::update(float value) {
	baseValue.setTargetValue(value);

	auto* out = valueBuffer.getWritePointer(0);

	// Fills the buffer with the base value
	for (int i = 0; i < valueBuffer.getNumSamples(); i++) {
		out[i] = baseValue.getNextValue();
	}
}

float ModDestination::getValue(int sampleIndex) const {
	return valueBuffer.getSample(0, sampleIndex);
}

void ModDestination::addMod(ModSource* source, float depth) {
	if (source == nullptr) return;
	auto* out = valueBuffer.getWritePointer(0);

	for (int i = 0; i < valueBuffer.getNumSamples(); i++) {
		out[i] += source->getValue(i) * depth;
	}
}
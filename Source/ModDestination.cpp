#include "ModDestination.h"

void ModDestination::prepare(const juce::dsp::ProcessSpec& spec) {
	baseValue.reset(spec.sampleRate, 0.02);
	valueBuffer.setSize(1, spec.maximumBlockSize);
}

float ModDestination::getValue(int sampleIndex) const {
	return valueBuffer.getSample(0, sampleIndex);
}
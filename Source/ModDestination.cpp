#include "ModDestination.h"

void ModDestination::prepare(const juce::dsp::ProcessSpec& spec) {
	baseValue.reset(spec.sampleRate, 0.02);
	valueBuffer.resize(spec.maximumBlockSize, baseValue.getTargetValue());
}

void ModDestination::process(const juce::dsp::AudioBlock<float>& block) {
	for (int i = 0; i < block.getNumSamples(); i++) valueBuffer[i] = baseValue.getNextValue();
}

void ModDestination::reset() {
	valueBuffer.clear();
}

void ModDestination::setBaseValue(float value) { baseValue.setTargetValue(value); }

float ModDestination::getValue(int index) const { return valueBuffer[index]; }

void ModDestination::addMod(ModSource* source, float depth) {
	if (source == nullptr) return;
	for (int i = 0; i < valueBuffer.size(); i++) valueBuffer[i] += source->getValue(i) * depth;
}
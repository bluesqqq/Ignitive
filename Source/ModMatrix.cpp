#include "ModMatrix.h"

void ModMatrix::prepare(const juce::dsp::ProcessSpec& spec) {
	for (auto& destination : destinationMap) destination.second->prepare(spec);
	for (auto& source : sourceMap) source.second->prepare(spec);
}

void ModMatrix::addDestination(const juce::String& id, const juce::String& displayName, juce::AudioProcessorValueTreeState& params) {
	destinationMap[id] = std::make_unique<ModDestination>(displayName);
	destinationIDs.push_back(id);
}

void ModMatrix::addSource(const juce::String& id, ModSource* modSource) {
	sourceMap[id] = modSource;
}
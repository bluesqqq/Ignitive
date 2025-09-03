#include "ModMatrix.h"

void ModMatrix::addSource(ModSource* source) { sources.push_back(source); }

void ModMatrix::addDestination(ModDestination* destination) { destinations.push_back(destination); }

ModConnection* ModMatrix::makeConnection(int sourceIndex, int destinationIndex, float depth) {
	if (!(sourceIndex >= 0 && sourceIndex < sources.size() && destinationIndex >= 0 && destinationIndex < destinations.size())) return nullptr;
	connections.push_back(ModConnection(sources[sourceIndex], destinations[destinationIndex], depth));
	return &connections.back();
}

ModConnection* ModMatrix::makeConnection(const juce::String& sourceID, const juce::String& destinationID, float depth) {
	ModSource* source = getSourceByID(sourceID);
	if (source == nullptr) return nullptr;

	ModDestination* destination = getDestinationByID(destinationID);
	if (destination == nullptr) return nullptr;

	connections.push_back(ModConnection(source, destination, depth));
	return &connections.back();
}

ModSource* ModMatrix::getSourceByID(const juce::String& sourceID) {
	auto it = std::find_if(sources.begin(), sources.end(), [&](const ModSource* source) { return source->sourceID == sourceID; });

	if (it != sources.end()) return *it;

	return nullptr;
}

ModDestination* ModMatrix::getDestinationByID(const juce::String& destinationID) {
	auto it = std::find_if(destinations.begin(), destinations.end(), [&](const ModDestination* destination) { return destination->destinationID == destinationID; });

	if (it != destinations.end()) return *it;

	return nullptr;
}

void ModMatrix::removeConnection(ModConnection* connection) {
	connections.remove_if([connection](const ModConnection& c) { return &c == connection; });
}

#include "ModMatrix.h"

void ModMatrix::prepare(const juce::dsp::ProcessSpec& spec) {
	for (auto& destination : destinationMap) destination.second->prepare(spec);
	for (auto& source : sourceMap) source.second->prepare(spec);
}

void ModMatrix::addDestination(const juce::String& id, const juce::String& displayName, juce::AudioProcessorValueTreeState& params) {
	destinationMap[id] = std::make_unique<ModDestination>(displayName);
}

void ModMatrix::addSource(const juce::String& id, ModSource* modSource) {
	sourceMap[id] = modSource;
}

ModSource* ModMatrix::getSource(const juce::String& id) {
	if (sourceMap.count(id)) return sourceMap[id];
	return nullptr;
}

ModDestination* ModMatrix::getDestination(const juce::String& id) {
	auto it = destinationMap.find(id);
	if (it != destinationMap.end()) return it->second.get();
	return nullptr;
}

void ModMatrix::process(const juce::dsp::AudioBlock<float>& block) {
	for (auto& destination : destinationMap) {
		float value = parameters.getRawParameterValue(destination.first)->load();
		destination.second->update(value);
	}

	for (ModConnection& connection : connections) {
		ModSource* source = getSource(connection.sourceID);
		ModDestination* destination = getDestination(connection.destinationID);

		if (source && destination) destination->addMod(source, connection.depth);
	}
}

int ModMatrix::getNumOfConnections() const { return connections.size(); }

float ModMatrix::getValue(const juce::String& destinationID, int sample) {
	// This runs every sample, which is a problem
	ModDestination* destination = getDestination(destinationID);

	if (!destination) return 0.0f;

	return juce::jlimit(0.0f, 1.0f, destination->getValue(sample));
}

std::vector<std::pair<juce::String, juce::String>> ModMatrix::getDestinationDisplayNameAndIDs() {
	std::vector<std::pair<juce::String, juce::String>> result;
	for (auto& d : destinationMap) {
		auto& id = d.first;
		auto& displayName = d.second->displayName;
		result.push_back(std::pair<juce::String, juce::String>(id, displayName));
	}

	return result;
}

bool ModMatrix::loadModConnectionsFromState(const juce::ValueTree& state) {
	auto mods = state.getChildWithName("ModConnections");
	if (!mods.isValid()) {
		connections.clear();
		return false;
	}

	connections.clear();

	for (int i = 0; i < mods.getNumChildren(); ++i) {
		auto conn = mods.getChild(i);
		if (conn.hasType("Connection"))
			connections.push_back({conn["source"].toString(), conn["destination"].toString(), (float)conn["depth"]});
	}

	return true;
}


void ModMatrix::saveModConnectionsToState(juce::ValueTree& state) {
	juce::ValueTree mods = state.getChildWithName("ModConnections");
	if (!mods.isValid())
		mods = juce::ValueTree("ModConnections");
	else
		mods.removeAllChildren(nullptr); 

	for (auto& c : connections) {
		juce::ValueTree conn("Connection");
		conn.setProperty("source", c.sourceID, nullptr);
		conn.setProperty("destination", c.destinationID, nullptr);
		conn.setProperty("depth", c.depth, nullptr);
		mods.addChild(conn, -1, nullptr);
	}

	if (!mods.getParent().isValid())
		state.addChild(mods, -1, nullptr);
}

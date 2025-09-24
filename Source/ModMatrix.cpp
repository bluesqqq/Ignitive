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

ModConnection* ModMatrix::makeConnection(const juce::String& sourceID, const juce::String& destinationID, float depth) {
	connections.push_back({ sourceID, destinationID, depth });
	return &connections.back();
}

bool ModMatrix::removeConnection(const juce::String& sourceID, const juce::String& destinationID) {
	auto it = std::remove_if(connections.begin(), connections.end(),
		[&](const ModConnection& conn) {
			return conn.sourceID == sourceID && conn.destinationID == destinationID;
		});

	if (it != connections.end()) {
		connections.erase(it, connections.end());
		return true;
	}

	return false;
}

ModSource* ModMatrix::getSource(const juce::String& id) {
	if (sourceMap.count(id)) return sourceMap[id];
	return nullptr;
}

ModDestination* ModMatrix::getDestination(const juce::String& id) {
	if (destinationMap.count(id)) return destinationMap[id].get();
	return nullptr;
}

void ModMatrix::update() {
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
	if (!mods.isValid()) return false;

	connections.clear();

	for (int i = 0; i < mods.getNumChildren(); ++i) {
		auto conn = mods.getChild(i);
		makeConnection(conn["source"].toString(), conn["destination"].toString(), (float)conn["depth"]);
	}

	return true;
}

void ModMatrix::saveModConnectionsToState(juce::ValueTree& state) {
	juce::ValueTree mods("ModConnections");

	for (auto& c : connections) {
		juce::ValueTree conn("Connection");
		conn.setProperty("source", c.sourceID, nullptr);
		conn.setProperty("destination", c.destinationID, nullptr);
		conn.setProperty("depth", c.depth, nullptr);
		mods.addChild(conn, -1, nullptr);
	}

	state.addChild(mods, -1, nullptr);
}
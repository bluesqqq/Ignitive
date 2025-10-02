#include "ModMatrix.h"

void ModMatrix::prepare(const juce::dsp::ProcessSpec& spec) {
	for (auto& destination : destinationMap) destination.second->prepare(spec);
	for (auto& source : sourceMap) source.second->prepare(spec);
}

void ModMatrix::addDestination(const juce::String& id, const juce::String& displayName) {
	destinationMap[id] = std::make_unique<ModDestination>(displayName);
}

void ModMatrix::addSource(const juce::String& id, ModSource* modSource) {
	sourceMap[id] = modSource;
}

ModSource* ModMatrix::getSource(const juce::String& id) {
	if (sourceMap.count(id)) return sourceMap[id];
	return nullptr;
}

int ModMatrix::numSources() { return sourceMap.size(); }

std::vector<ModConnection>& ModMatrix::getConnections() { return connections; }

std::vector<ModConnection*> ModMatrix::getConnectionsWithSource(const juce::String& sourceID) {
	std::vector<ModConnection*> result;

	for (auto& connection : connections)
		if (connection.sourceID == sourceID)
			result.push_back(&connection);

	return result;
}

ModDestination* ModMatrix::getDestination(const juce::String& id) {
	auto it = destinationMap.find(id);
	if (it != destinationMap.end()) return it->second.get();
	return nullptr;
}

void ModMatrix::process(const juce::dsp::AudioBlock<float>& block) {
	for (auto& [destinationID, destination] : destinationMap) {
		float value = parameters.getRawParameterValue(destinationID)->load();
		destination->setBaseValue(value);
		destination->process(block);
	}

	for (ModConnection& connection : connections) {
		ModSource* source = getSource(connection.sourceID);
		ModDestination* destination = getDestination(connection.destinationID);

		if (source && destination) destination->addMod(source, connection.depth);
	}
}

void ModMatrix::reset() {

}

void ModMatrix::setEmptyConnections() {
	connections.clear();

	for (const auto& [sourceID, source] : sourceMap)
		for (int i = 0; i < numDestinations(); ++i)
			connections.push_back({ sourceID, "", 0.0f });
}

float ModMatrix::getValue(const juce::String& destinationID, int sample) {
	// This runs every sample, which is a problem
	ModDestination* destination = getDestination(destinationID);

	if (!destination) return 0.0f;

	return juce::jlimit(0.0f, 1.0f, destination->getValue(sample));
}

std::vector<juce::String> ModMatrix::getDestinationsIDs() {
	std::vector<juce::String> result;

	for (const auto& pair : destinationMap) result.push_back(pair.first);

	return result;
}

std::vector<juce::String> ModMatrix::getAvailableDestinationIDs(juce::String sourceID) {
	std::vector<juce::String> result = getDestinationsIDs();

	// Remove already used destinations
	for (const auto& connection : connections)
		if (connection.sourceID == sourceID)
			result.erase(std::remove(result.begin(), result.end(), connection.destinationID), result.end());

	return result;
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

void ModMatrix::randomizeConnections() {
	setEmptyConnections();

	static thread_local std::mt19937_64 rng{ std::random_device{}() };

	std::binomial_distribution<int> connectionDist(numDestinations(), 0.2);

	std::normal_distribution<float> depthDist(0.0f, 0.2f);

	for (const auto& [sourceID, source] : sourceMap) {
		auto& conns = getConnectionsWithSource(sourceID);

		int numConnections = std::min(connectionDist(rng), (int)conns.size());

		for (int i = 0; i < numConnections; i++) {
			auto availableDestinations = getAvailableDestinationIDs(sourceID);
			if (availableDestinations.empty()) break;

			std::uniform_int_distribution<int> destDist(0, (int)availableDestinations.size() - 1);
			juce::String destinationID = availableDestinations[destDist(rng)];
			conns[i]->destinationID = destinationID;

			float depth = depthDist(rng);
			conns[i]->depth = juce::jlimit(-1.0f, 1.0f, depth);
		}
	}
}
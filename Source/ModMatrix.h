#pragma once

#include "ModSource.h"
#include "ModDestination.h"
#include "Parameters.h"

struct ModConnection {
	juce::String sourceID;
	juce::String destinationID;
	float depth;
};

class ModMatrix {
	private:
		juce::AudioProcessorValueTreeState& parameters;

		std::unordered_map<juce::String, std::unique_ptr<ModDestination>> destinationMap;
		std::unordered_map<juce::String, ModSource*> sourceMap;

		std::vector<ModConnection> connections;

		std::vector<juce::String> destinationIDs;

	public:
		ModMatrix(juce::AudioProcessorValueTreeState& parameters) : parameters(parameters) {}

		void prepare(const juce::dsp::ProcessSpec& spec);

		void addDestination(const juce::String& id, const juce::String& displayName, juce::AudioProcessorValueTreeState& parameters);

		void addSource(const juce::String& id, ModSource* modSource);

		ModConnection* makeConnection(const juce::String& sourceID, const juce::String& destinationID, float depth = 0.5f) {
			connections.push_back({ sourceID, destinationID, depth });
			return &connections.back();
		}

		bool removeConnection(const juce::String& sourceID, const juce::String& destinationID) {
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

		ModSource* getSourceByID(const juce::String& id) {
			if (sourceMap.count(id)) return sourceMap[id];
			return nullptr;
		}

		ModDestination* getDestinationByID(const juce::String& id) {
			if (destinationMap.count(id)) return destinationMap[id].get();
			return nullptr;
		}

		void update() {
			for (auto& destination : destinationMap) {
				float value = parameters.getRawParameterValue(destination.first)->load();
				destination.second->update(value);
			}

			for (ModConnection& connection : connections) {
				ModSource* source = getSourceByID(connection.sourceID);
				ModDestination* destination = getDestinationByID(connection.destinationID);

				if (source && destination) destination->addMod(source, connection.depth);
			}
		}

		int getNumOfConnections() const { return connections.size(); }

		float getValue(const juce::String& destinationID, int sample) {
			ModDestination* destination = getDestinationByID(destinationID);

			if (!destination) return 0.0f;

			return juce::jlimit(0.0f, 1.0f, destination->getValue(sample));
		}

		std::vector<ModConnection>& getConnections() { return connections; }

		std::vector<juce::String>& getDestinationsIDs() { return destinationIDs; }

		std::vector<std::pair<juce::String, juce::String>> getDestinationDisplayNameAndIDs() {
			std::vector<std::pair<juce::String, juce::String>> result;
			for (auto& d : destinationMap) {
				auto& id = d.first;
				auto& displayName = d.second->displayName;
				result.push_back(std::pair<juce::String, juce::String>(id, displayName));
			}

			return result;
		}

		bool loadModConnectionsFromState(const juce::ValueTree& state) {
			auto mods = state.getChildWithName("ModConnections");
			if (!mods.isValid()) return false;

			connections.clear();

			for (int i = 0; i < mods.getNumChildren(); ++i) {
				auto conn = mods.getChild(i);
				makeConnection(conn["source"].toString(), conn["destination"].toString(), (float)conn["depth"]);
			}

			return true;
		}

		void saveModConnectionsToState(juce::ValueTree& state) {
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

		void randomizeConnections() {
			// Todo: Implement this
		}
};
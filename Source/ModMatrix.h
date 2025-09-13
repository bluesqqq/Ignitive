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

		void addDestination(const juce::String& id, juce::AudioProcessorValueTreeState& parameters);

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
};
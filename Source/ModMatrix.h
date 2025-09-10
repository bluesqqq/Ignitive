#pragma once

#include "ModSource.h"
#include "ModDestination.h"
#include "Parameters.h"

struct ModConnection {
	ModSource* source;
	ModDestination* destination;
	float depth = 1.0f;

	ModConnection(ModSource* source, ModDestination* destination, float depth = 0.5f);
};

class ModMatrix {
	private:
		std::unordered_map<juce::String, std::unique_ptr<ModDestination>> destinationMap;
		std::unordered_map<juce::String, ModSource*> sourceMap;
		std::list<ModConnection> connections;

	public:
		void prepare(const juce::dsp::ProcessSpec& spec) {
			for (auto& destination : destinationMap) {
				destination.second->prepare(spec);
			}

			for (auto& source : sourceMap) {
				source.second->prepare(spec);
			}
		}

		void addDestination(const juce::String& id, juce::AudioProcessorValueTreeState& parameters) {
			juce::RangedAudioParameter* parameter = parameters.getParameter(id); // this line throwing an error

			if (parameter != nullptr) 
				destinationMap[id] = std::make_unique<ModDestination>(parameter);
		}

		void addSource(const juce::String& id, ModSource* modSource) {
			sourceMap[id] = modSource;
		}

		ModConnection* makeConnection(const juce::String& sourceID, const juce::String& destinationID, float depth = 0.5f) {
			ModSource* source = getSourceByID(sourceID);
			ModDestination* destination = getDestinationByID(destinationID);

			if (source && destination) {
				connections.push_back(ModConnection(source, destination, depth));
				return &connections.back();
			}
			return nullptr;
		}

		ModSource* getSourceByID(const juce::String& id) {
			if (sourceMap.count(id)) return sourceMap[id];
			return nullptr;
		}

		ModDestination* getDestinationByID(const juce::String& id) {
			if (destinationMap.count(id)) return destinationMap[id].get();
			return nullptr;
		}

		void removeConnection(ModConnection* connection);

		std::list<ModConnection>& getConnections() { return connections; }

		void update() {
			for (auto& destination : destinationMap) destination.second->update();

			for (auto& conn : connections) {
				if (conn.source && conn.destination) {
					conn.destination->addMod(conn.source);
				}
			}
		}

		void clearConnections() { connections.clear(); }

		int getNumOfConnections() const { return static_cast<int>(connections.size()); }

		float getValue(const juce::String& destinationID, int sample) {
			ModDestination* destination = getDestinationByID(destinationID);

			if (!destination) return 0.0f;

			return destination->getValue(sample);

		}

		
};
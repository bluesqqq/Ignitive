#pragma once

#include "ModSource.h"

class ModMatrix {
	private:
		std::vector<ModSource*> sources;
		std::vector<ModDestination*> destinations;
		std::list<ModConnection> connections;

	public:
		void addSource(ModSource* source);

		void addDestination(ModDestination* destination);

		std::vector<ModSource*>& getSources() { return sources; }
		std::vector<ModDestination*>& getDestinations() { return destinations; }

		ModConnection* makeConnection(int sourceIndex, int destinationIndex, float depth = 0.5f);

		ModConnection* makeConnection(const juce::String& sourceID, const juce::String& destinationID, float depth = 0.5f);

		ModConnection* makeConnection(ModSource* source, ModDestination* destination, float depth = 0.5f);

		ModSource* getSourceByID(const juce::String& sourceID);

		ModDestination* getDestinationByID(const juce::String& destinationID);

		void removeConnection(ModConnection* connection);

		std::list<ModConnection>& getConnections() { return connections; }

		void update() {
			for (auto destination : destinations) destination->modAmount = 0.0f;

			for (auto& conn : connections) {
				if (conn.source && conn.destination) { //TODO: FIX THIS BACK
					conn.destination->modAmount += 0; //conn.source->getNextValue() * conn.depth;
				}
			}
		}

		void clearConnections() { connections.clear(); }

		int getNumOfConnections() const { return static_cast<int>(connections.size()); }
};
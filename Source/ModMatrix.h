#pragma once

#include "ModSource.h"
#include "ModDestination.h"
#include "Parameters.h"
#include <random>

struct ModConnection {
	juce::String sourceID;
	juce::String destinationID;
	float depth;
};

class ModMatrix {
	private:
		juce::AudioProcessorValueTreeState& parameters;


		std::unordered_map<juce::String, std::unique_ptr<ModDestination>> destinationMap; // [destinationID] -> ModDestination
		std::unordered_map<juce::String, ModSource*> sourceMap; // [sourceID] -> ModSource*

		/// <summary>
		/// List of all active modulation connections
		/// </summary>
		std::vector<ModConnection> connections;

	public:
		ModMatrix(juce::AudioProcessorValueTreeState& parameters) : parameters(parameters) {}

		void prepare(const juce::dsp::ProcessSpec& spec);
		void update();

		void addDestination(const juce::String& id, const juce::String& displayName, juce::AudioProcessorValueTreeState& parameters);
		ModDestination* getDestination(const juce::String& id);
		int numDestinations() { return destinationMap.size(); }

		void addSource(const juce::String& id, ModSource* modSource);
		ModSource* getSource(const juce::String& id);
		int numSources() { return sourceMap.size(); }

		std::vector<ModConnection>& getConnections() { return connections; }
		std::vector<ModConnection*> getConnectionsWithSource(const juce::String& sourceID) {
			std::vector<ModConnection*> result;

			for (auto& connection : connections)
				if (connection.sourceID == sourceID)
					result.push_back(&connection);

			return result;
		}
		int getNumOfConnections() const;
		void setEmptyConnections() {
			connections.clear();

			for (const auto& [sourceID, source] : sourceMap)
				for (int i = 0; i < numDestinations(); ++i)
					connections.push_back({sourceID, "", 0.0f});
		}

		float getValue(const juce::String& destinationID, int sample);

		std::vector<juce::String> getDestinationsIDs() {
			std::vector<juce::String> result;

			for (const auto& pair : destinationMap) result.push_back(pair.first);

			return result;
		}

		std::vector<juce::String> getAvailableDestinationIDs(juce::String sourceID) {
			std::vector<juce::String> result = getDestinationsIDs();

			// Remove already used destinations
			for (const auto& connection : connections)
				if (connection.sourceID == sourceID)
					result.erase(std::remove(result.begin(), result.end(), connection.destinationID), result.end());

			return result;
		}

		std::vector<std::pair<juce::String, juce::String>> getDestinationDisplayNameAndIDs();

		bool loadModConnectionsFromState(const juce::ValueTree& state);
		void saveModConnectionsToState(juce::ValueTree& state);

		void randomizeConnections() {
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


};
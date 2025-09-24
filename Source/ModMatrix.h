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
		void update();

		void addDestination(const juce::String& id, const juce::String& displayName, juce::AudioProcessorValueTreeState& parameters);
		void addSource(const juce::String& id, ModSource* modSource);
		ModSource* getSource(const juce::String& id);
		ModDestination* getDestination(const juce::String& id);

		ModConnection* makeConnection(const juce::String& sourceID, const juce::String& destinationID, float depth = 0.5f);
		bool removeConnection(const juce::String& sourceID, const juce::String& destinationID);
		int getNumOfConnections() const;
		std::vector<ModConnection>& getConnections() { return connections; }

		float getValue(const juce::String& destinationID, int sample);

		std::vector<juce::String>& getDestinationsIDs() { return destinationIDs; }
		std::vector<std::pair<juce::String, juce::String>> getDestinationDisplayNameAndIDs();

		bool loadModConnectionsFromState(const juce::ValueTree& state);
		void saveModConnectionsToState(juce::ValueTree& state);

		void randomizeConnections() {
			// Todo: Implement this
		}
};
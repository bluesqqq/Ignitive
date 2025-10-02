#pragma once

#include "ModSource.h"
#include "ModDestination.h"
#include "Parameters.h"
#include <random>

/*
	Represents a single modulation connection between a source and a destination

	Each ModConnection stores the IDs of the modulation source (ModSource), and a destination
	(ModDestination), along with a depth value that determines the strength of the modulation.
*/
struct ModConnection {
	// The modulation source ID
	juce::String sourceID;
	// The destination source ID (same as the parameter ID in apvts)
	juce::String destinationID;
	// The depth of the connection (0.0 - 1.0)
	float depth;
};

/*
	This class defines a modulation matrix that can manage connections between registered
	sources and destinations.

	ModMatrix allows registering sources (ModSource) and destinations (ModDestination),
	and manages the connections (ModConnection) between them. Each source can modulate
	multiple destinations. The class handles preparing, processing, resetting, and
	storing modulation data

	@see ModSource
	@see ModDestination
	@see ModConnection
*/
class ModMatrix {
	/*
		When I made this class, I wanted something similar to Serum's mod matrix. This is a more
		dumbed-down version with simpler features, but it still mostly achieves what I want.

		I deciced on having empty (destination-less) connections as I found it easier to select
		destinations than fumbling with adding/removing connections. Sure, it limits the number
		of connections (per source) to the number of destinations, but is there really any point
		in, say, mapping ENV -> DRIVE twice? Serum lets you do it, but theres no real benefit
		to doing so (just my opinion).
	*/
	private:
		juce::AudioProcessorValueTreeState& parameters;

		std::unordered_map<juce::String, std::unique_ptr<ModDestination>> destinationMap; // [destinationID] -> ModDestination
		std::unordered_map<juce::String, ModSource*> sourceMap; // [sourceID] -> ModSource*

		// List of all active modulation connections
		std::vector<ModConnection> connections;

	public:
		ModMatrix(juce::AudioProcessorValueTreeState& parameters) : parameters(parameters) {}

		void prepare(const juce::dsp::ProcessSpec& spec);
		void process(const juce::dsp::AudioBlock<float>& block);
		void reset();

		// Adds a destination to the mod matrix.
		void addDestination(const juce::String& id, const juce::String& displayName);
		// Returns a pointer to the ModDestination with the given ID, or nullptr if not found.
		ModDestination* getDestination(const juce::String& id);
		// Returns the number of registered destinations.
		int numDestinations() { return destinationMap.size(); }

		// Adds a source to the mod matrix.
		void addSource(const juce::String& id, ModSource* modSource);
		// Returns a pointer to the ModSource with the given ID, or nullptr if not found.
		ModSource* getSource(const juce::String& id);
		// Returns the number of registered sources.
		int numSources();

		// Returns a vector of all connections.
		std::vector<ModConnection>& getConnections();
		// Returns a vector of pointers to all connections with the given source ID.
		std::vector<ModConnection*> getConnectionsWithSource(const juce::String& sourceID);
		// Clears all connections. Creates new empty connections, such that each source has n empty connections (n being the number of destinations registered).
		void setEmptyConnections();

		// Returns the current value of the given destination at the given sample index.
		float getValue(const juce::String& destinationID, int sample);

		// Returns a vector of all destination IDs
		std::vector<juce::String> getDestinationsIDs();

		// Returns a vector of all available (not used) destination IDs
		std::vector<juce::String> getAvailableDestinationIDs(juce::String sourceID);

		std::vector<std::pair<juce::String, juce::String>> getDestinationDisplayNameAndIDs();

		bool loadModConnectionsFromState(const juce::ValueTree& state);
		void saveModConnectionsToState(juce::ValueTree& state);

		// Clears all previous connections and creates random connections.
		void randomizeConnections();
};
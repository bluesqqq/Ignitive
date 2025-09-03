#pragma once

#include <vector>
#include <JuceHeader.h>

class ModSource {
	public:
		virtual float getNextValue() = 0;
		virtual ~ModSource() = default;
};

struct ModDestination {
	juce::String name;
	float baseValue = 0.0f;     // from APVTS
	float modAmount = 0.0f;     // sum of modulation sources * depth
	ModDestination(const juce::String& n) : name(n) {}
	float getFinalValue() const { return baseValue + modAmount; }
};

struct ModConnection {
	ModSource* source;
	ModDestination* destination;
	float depth = 1.0f;
};

class ModMatrix {
	private:
		std::list<ModConnection> connections;

	public:
		ModConnection* makeConnection(ModSource* src, ModDestination* dst, float depth) {
			connections.push_back({ src, dst, depth });
			return &connections.back();
		}

		void removeConnection(ModConnection* conn) {
			connections.remove_if([conn](const ModConnection& c) { return &c == conn; });
		}

		const std::list<ModConnection>& getConnections() const {
			return connections;
		}

		void update() {
			resetDestinations();

			for (auto& conn : connections) {
				if (conn.source && conn.destination) {
					conn.destination->modAmount += conn.source->getNextValue() * conn.depth;
				}
			}
		}

		void resetDestinations() {
			for (auto& conn : connections) {
				if (conn.destination)
					conn.destination->modAmount = 0.0f;
			}
		}

		void clearConnections() {
			connections.clear();
		}

		int getNumOfConnections() const {
			return static_cast<int>(connections.size());
		}
};
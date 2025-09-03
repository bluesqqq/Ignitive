#pragma once

#include <vector>
#include <JuceHeader.h>

struct ModSource {
	juce::String name;
	juce::String sourceID;

	ModSource(const juce::String& name, const juce::String& sourceID);

	virtual float getNextValue() = 0;
	virtual ~ModSource() = default;
};

struct ModDestination {
	juce::String name;
	juce::String destinationID;
	float minValue = 0.0f;
	float maxValue = 1.0f;

	float baseValue = 0.0f;
	float modAmount = 0.0f;

	ModDestination(const juce::String& name, const juce::String& destinationID, float minValue = 0.0f, float maxValue = 1.0f);

	float getFinalValue() const;
	float getRange() const;
};

struct ModConnection {
	ModSource* source;
	ModDestination* destination;
	float depth = 1.0f;

	ModConnection(ModSource* source, ModDestination* destination, float depth);
};
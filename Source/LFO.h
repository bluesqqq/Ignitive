#pragma once

#include "ModSource.h"
#include "ParametersDisplay.h"

class LFO : public ModSource, public ParametersDisplayInterface {
	private:
		juce::AudioProcessorValueTreeState& parameters;

		float phase = 0.0f;
		float speedHz = 1.0f;

		juce::String speedID;

	public:
		LFO(juce::AudioProcessorValueTreeState& parameters, const juce::String& speedID);

		void update();
};
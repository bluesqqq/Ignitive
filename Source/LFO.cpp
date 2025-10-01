#include "LFO.h"

LFO::LFO(juce::AudioProcessorValueTreeState& params, const juce::String& speedID)
	: ModSource(juce::Colours::orange, {-1.0f, 1.0f}), 
	  ParametersDisplayInterface({ "Speed" }),
	  parameters(params), speedID(speedID) {
	valueGetters.push_back([this]() { return juce::String(speedHz, 1) + " hz"; });
}

void LFO::update() {
	speedHz = juce::jmap(parameters.getRawParameterValue(speedID)->load(), 0.1f, 10.0f);

	const int numSamples = modulationBuffer.getNumSamples();
	float* buffer = modulationBuffer.getWritePointer(0);

	const float phaseIncrement = 2.0f * juce::MathConstants<float>::pi * speedHz / sampleRate;

	for (int i = 0; i < numSamples; i++) {
		buffer[i] = std::sin(phase);
		writeToFifo(buffer[i]);
		phase += phaseIncrement;
		if (phase > 2.0f * juce::MathConstants<float>::pi) phase -= 2.0f * juce::MathConstants<float>::pi;
	}
}
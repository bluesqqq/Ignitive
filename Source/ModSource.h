#pragma once

#include <JuceHeader.h>

class ModSource {
	protected:
		juce::AudioBuffer<float> modulationBuffer;

	public:
		virtual void prepare(const juce::dsp::ProcessSpec& spec);

		float getValue(int sample) const;
};
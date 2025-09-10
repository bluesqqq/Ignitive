#pragma once

#include <vector>
#include <JuceHeader.h>

class ModSource {
	protected:
		juce::AudioBuffer<float> modulationBuffer;

	public:
		virtual void prepare(const juce::dsp::ProcessSpec& spec) {
			modulationBuffer.setSize(1, spec.maximumBlockSize);
		}

		float getValue(int sampleIndex) const {
			return modulationBuffer.getSample(0, sampleIndex);
		}
};
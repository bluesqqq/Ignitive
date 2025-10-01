#pragma once

#include <JuceHeader.h>

class ModSource {
	protected:
		juce::AudioBuffer<float> modulationBuffer;
		float sampleRate = 0;

		juce::AbstractFifo fifo{ 8192 };
		std::vector<float> fifoBuffer;

		void writeToFifo(float value);

	public:
		virtual void prepare(const juce::dsp::ProcessSpec& spec);
		virtual void reset();
		float getValue(int sample) const;

		bool popFifo(int samples);
		float readFifo();
};
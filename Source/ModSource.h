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
		juce::Colour displayColor;
		juce::Range<float> displayRange;

		ModSource(const juce::Colour& displayColor, const juce::Range<float>& displayRange);

		virtual void prepare(const juce::dsp::ProcessSpec& spec);
		virtual void process(const juce::dsp::AudioBlock<float>& block) = 0;
		virtual void reset();

		float getValue(int sample) const;

		bool popFifo(int samples);
		float readFifo();
};
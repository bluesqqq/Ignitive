#pragma once

#include <JuceHeader.h>

/*
	Represents a modulation source that generates per-sampe modulation values.

	ModSource is the base class for all modulation source (LFOs, envelopes, etc.).
	It provides a buffer for storing values, handles block-based processing, and 
	supports a FIFO for passing values safely to GUI components.
*/
class ModSource {
	protected:
		// Buffer holding per-sample modulation values 
		std::vector<float> modulationBuffer;
		float sampleRate = 0;

		juce::AbstractFifo fifo{ 8192 };
		std::vector<float> fifoBuffer;

		void writeToFifo(float value);

	public:
		// The color this source appears in GUI components
		juce::Colour displayColor;
		// The range this source displays in GUI components
		juce::Range<float> displayRange;

		ModSource(const juce::Colour& displayColor, const juce::Range<float>& displayRange);

		virtual void prepare(const juce::dsp::ProcessSpec& spec);
		virtual void process(const juce::dsp::AudioBlock<float>& block) = 0;
		virtual void reset();

		// Returns the current value at the given sample index
		float getValue(int sample) const;

		// Pops a specified number of samples from the FIFO. If there are not enough samples, it will not remove any and return false.
		bool popFifo(int samples);

		// Reads a sample from the FIFO
		float readFifo();
};
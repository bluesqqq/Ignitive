#pragma once

#include "ModSource.h"

class LFO : public ModSource {
	private:
		juce::AudioProcessorValueTreeState& parameters;

		float phase = 0.0f;
		float speedHz = 1.0f;
		float sampleRate = 44100.0f;

		juce::String speedID;

		juce::AbstractFifo fifo{ 8192 };
		std::vector<float> fifoBuffer;

	public:
		LFO(juce::AudioProcessorValueTreeState& parameters, const juce::String& speedID) : parameters(parameters), speedID(speedID) {
			fifoBuffer.resize(fifo.getTotalSize());
			writeToFifo(0.0f);
		}

		void prepare(const juce::dsp::ProcessSpec& spec) override {
			ModSource::prepare(spec);
			sampleRate = spec.sampleRate;
		}

		void update() {
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

		void writeToFifo(float value) {
			int start1, size1, start2, size2;
			fifo.prepareToWrite(1, start1, size1, start2, size2);

			if (size1 > 0)
				fifoBuffer[start1] = value;
			if (size2 > 0)
				fifoBuffer[start2] = value;

			fifo.finishedWrite(1);
		}


		bool popFifo(int samples) {
			if (fifo.getNumReady() > samples) {
				int start1, size1, start2, size2;
				fifo.prepareToRead(samples, start1, size1, start2, size2);

				fifo.finishedRead(samples);
				return true;
			}
			return false;
		}

		float readFifo() {
			float value = 0.0f;

			if (fifo.getNumReady() > 0) {
				int start1, size1, start2, size2;
				fifo.prepareToRead(1, start1, size1, start2, size2);

				if (size1 > 0)
					value = fifoBuffer[start1];
				else if (size2 > 0)
					value = fifoBuffer[start2];

				fifo.finishedRead(1);
			}

			return value;
		}
};
#pragma once

#include <vector>
#include "ModSource.h"


class EnvelopeFollower : public ModSource {
	private:
		void updateCoefficients();

		float attackTime, releaseTime;
		float gate;
		float sampleRate;
		float attackCoef, releaseCoef;

		float currentValue = 0;

		juce::AbstractFifo fifo{ 8192 };
		std::vector<float> fifoBuffer;

		float processSample(float sample) {
			if (sample > currentValue && sample > gate)
				currentValue = attackCoef * currentValue + (1.0f - attackCoef) * sample;
			else
				currentValue = releaseCoef * currentValue + (1.0f - releaseCoef) * sample;

			return currentValue;
		}

	public:
		EnvelopeFollower(float attackTime = 0.001f, float releaseTime = 0.5f, float sampleRate = 44100.0f);

		void setAttackTime(float attack);
		void setReleaseTime(float release);
		void setSampleRate(float rate);
		void setGate(float g);

		void process(const juce::dsp::AudioBlock<float>& block) {
			auto numSamples  = (int)block.getNumSamples();
			auto numChannels = (int)block.getNumChannels();

			auto* out = modulationBuffer.getWritePointer(0);

			for (int sample = 0; sample < numSamples; ++sample) {
				float s = 0.0f;

				for (int channel = 0; channel < numChannels; ++channel)
					s += std::abs(block.getSample(channel, sample));

				s /= (float)numChannels;

				out[sample] = processSample(s);

				writeToFifo(out[sample]);
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
			// We only pop if we have at least samples + 1 available
			if (fifo.getNumReady() > samples) {
				int start1, size1, start2, size2;
				fifo.prepareToRead(samples, start1, size1, start2, size2);

				// we don’t actually care about the values, just skip them
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
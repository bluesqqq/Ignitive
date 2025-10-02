#include "ModSource.h"

ModSource::ModSource(const juce::Colour& color, const juce::Range<float>& range) : displayColor(color), displayRange(range) { }

void ModSource::prepare(const juce::dsp::ProcessSpec& spec) {
	modulationBuffer.resize(spec.maximumBlockSize, 0.0f);
	sampleRate = spec.sampleRate;
	fifoBuffer.resize(fifo.getTotalSize());
	writeToFifo(0.0f);
}

float ModSource::getValue(int sample) const { return modulationBuffer[sample]; }

void ModSource::reset() { modulationBuffer.clear(); }

void ModSource::writeToFifo(float value) {
	int start1, size1, start2, size2;
	fifo.prepareToWrite(1, start1, size1, start2, size2);

	if (size1 > 0)
		fifoBuffer[start1] = value;
	if (size2 > 0)
		fifoBuffer[start2] = value;

	fifo.finishedWrite(1);
}

bool ModSource::popFifo(int samples) {
	if (fifo.getNumReady() > samples) {
		int start1, size1, start2, size2;
		fifo.prepareToRead(samples, start1, size1, start2, size2);

		fifo.finishedRead(samples);
		return true;
	}
	return false;
}

float ModSource::readFifo() {
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
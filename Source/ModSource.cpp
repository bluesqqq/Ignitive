#include "ModSource.h"

void ModSource::prepare(const juce::dsp::ProcessSpec& spec) { modulationBuffer.setSize(1, spec.maximumBlockSize); }

float ModSource::getValue(int sample) const { return modulationBuffer.getSample(0, sample); }
#pragma once

#define _USE_MATH_DEFINES

#include <vector>
#include <JuceHeader.h>
#include <cmath>
#include "ModMatrix.h"

enum class DistortionType {
	HardClip,
	Tube,
	Fuzz,
	Rectify,
	Downsample,
	NumDefinitions
};

struct DistortionDefinition {
	const char* name;
	float (*process)(float, float);
};

class DistortionProcessor : public juce::dsp::ProcessorBase {
	private:
		juce::AudioProcessorValueTreeState& parameters;
		ModMatrix& modMatrix;

		std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;

		float distort(float sample, float drive, float color);

		int index;

		juce::String driveID;
		juce::String colorID;
		juce::String typeID;

	public:
		static const std::array<DistortionDefinition, 7> distortionDefs;

		DistortionProcessor(juce::AudioProcessorValueTreeState& params, ModMatrix& modMatrix, const juce::String& driveID, const juce::String& colorID, const juce::String& typeID);

		void prepare(const juce::dsp::ProcessSpec& spec) override;
		void process(const juce::dsp::ProcessContextReplacing<float>& context) override;
		void reset() override;

		void setDistortionAlgorithm(DistortionType type);

		void updateParameters();

		std::vector<float> getWaveshape();

		float getModifiedDriveValue() {
			return modMatrix.getValue(driveID, 0);
		}
};
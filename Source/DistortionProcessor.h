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
	Downsample
};

class DistortionProcessor : public juce::dsp::ProcessorBase {
	private:
		juce::AudioProcessorValueTreeState& parameters;
		ModMatrix& modMatrix;

		std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;

		float hardClip(float sample, float drive);

		float tube(float sample, float drive);

		float fuzz(float sample, float drive);

		float rectify(float sample, float drive);

		float downsample(float sample, float drive);

		float distort(float sample, float drive, float color);

		DistortionType type;

		juce::String driveID;
		juce::String colorID;
		juce::String typeID;

	public:
		DistortionProcessor(juce::AudioProcessorValueTreeState& params, ModMatrix& modMatrix, const juce::String& driveID, const juce::String& colorID, const juce::String& typeID);

		void prepare(const juce::dsp::ProcessSpec& spec) override;
		void process(const juce::dsp::ProcessContextReplacing<float>& context) override;
		void reset() override;

		void setDistortionAlgorithm(DistortionType type);

		void updateParameters();

		std::vector<float> getWaveshape();
};
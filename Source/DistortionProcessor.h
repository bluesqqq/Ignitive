#pragma once

#define _USE_MATH_DEFINES

#include <vector>
#include <JuceHeader.h>
#include <cmath>

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

		std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;

		float hardClip(float sample, float drive);

		float tube(float sample, float drive);

		float fuzz(float sample, float drive);

		float rectify(float sample, float drive);

		float downsample(float sample, float drive);

		float distort(float sample, float drive);

		DistortionType type;

		juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> drive;
		juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> color;

		juce::String driveID;
		juce::String colorID;
		juce::String typeID;

	public:
		DistortionProcessor(juce::AudioProcessorValueTreeState& params, const juce::String& driveID, const juce::String& colorID, const juce::String& typeID);

		void prepare(const juce::dsp::ProcessSpec& spec) override;
		void process(const juce::dsp::ProcessContextReplacing<float>& context) override;
		void reset() override;

		void setDistortionAlgorithm(DistortionType type);

		std::vector<float> getWaveshape();
};
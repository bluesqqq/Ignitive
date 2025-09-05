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

		float hardClip(float sample);

		float tube(float sample);

		float fuzz(float sample);

		float rectify(float sample);

		float downsample(float sample);

		float distort(float sample);

		DistortionType type;
		float drive;

	public:
		DistortionProcessor(juce::AudioProcessorValueTreeState& params);

		void prepare(const juce::dsp::ProcessSpec& spec) override;
		void process(const juce::dsp::ProcessContextReplacing<float>& context) override;
		void reset() override;

		void setDistortionAlgorithm(DistortionType type);

		void setDrive(float newDrive);

		float getDrive();

		std::vector<float> getWaveshape();

		float processSample(float sample);
};
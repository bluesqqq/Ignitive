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

class DistortionEngine {
	private:
		float hardClip(float sample);

		float tube(float sample);

		float fuzz(float sample);

		float rectify(float sample);

		float downsample(float sample);

		float distort(float sample);

		DistortionType type;
		float drive;

	public:
		DistortionEngine();

		void setDistortionAlgorithm(DistortionType type);

		void setDrive(float newDrive);

		float getDrive();

		std::vector<float> getWaveshape();

		float processSample(float sample);
};
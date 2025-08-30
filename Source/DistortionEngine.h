#pragma once

#define _USE_MATH_DEFINES

#include <vector>
#include <JuceHeader.h>
#include <cmath>

class DistortionEngine {
public:
	DistortionEngine();

	void setDistortionAlgorithm(int algoritm);

	void setDrive(float newDrive);

	void setModulation(float newModulation);

	float getDrive();

	std::vector<float> getWaveshape();

	float processSample(float sample);

private:
	float hardClip(float sample);

	float tube(float sample);

	float fuzz(float sample);

	float rectify(float sample);

	float downsample(float sample);

	float distort(float sample);

	int distortionAlgorithm;
	float drive;
	float modulation; // from 0.0 - 1.0

};
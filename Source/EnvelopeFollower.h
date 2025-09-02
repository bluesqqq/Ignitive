#pragma once

#include <vector>

class EnvelopeFollower {
	public:
		EnvelopeFollower(float attackTime = 0.001f, float releaseTime = 0.5f, float sampleRate = 44100.0f);

		void setAttackTime(float attack);
		void setReleaseTime(float release);
		void setSampleRate(float rate);
		void setGate(float g);

		float process(float input);

		float getEnvelope() const;
		float getGate() const;

	private:
		void updateCoefficients();

		float attackTime, releaseTime;
		float gate;
		float sampleRate;
		float attackCoef, releaseCoef;
		float envelope;
};
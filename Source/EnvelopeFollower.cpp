#include "EnvelopeFollower.h"
#include <cmath>

EnvelopeFollower::EnvelopeFollower(float attackTime, float releaseTime, float sampleRate)
    : attackTime(attackTime), releaseTime(releaseTime), sampleRate(sampleRate), envelope(0.0f) {
    updateCoefficients();
}

void EnvelopeFollower::setAttackTime(float attack) {
    attackTime = attack;
    updateCoefficients();
}

void EnvelopeFollower::setReleaseTime(float release) {
    releaseTime = release;
    updateCoefficients();
}

void EnvelopeFollower::setGate(float g) { gate = g; }

void EnvelopeFollower::setSampleRate(float rate) {
    sampleRate = rate;
    updateCoefficients();
}

float EnvelopeFollower::process(float input) {
    float absInput = std::abs(input);

    if (absInput > envelope && absInput > gate)
        envelope = attackCoef * envelope + (1.0f - attackCoef) * absInput;  // Attack phase
    else
        envelope = releaseCoef * envelope + (1.0f - releaseCoef) * absInput; // Release phase

    return envelope;
}

float EnvelopeFollower::getEnvelope() const { return envelope; }

float EnvelopeFollower::getGate() const { return gate; }

void EnvelopeFollower::updateCoefficients() {
    attackCoef = std::exp(-std::log(9.0f) / (attackTime * sampleRate));
    releaseCoef = std::exp(-std::log(9.0f) / (releaseTime * sampleRate));
}
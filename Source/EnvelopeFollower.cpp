#include "EnvelopeFollower.h"
#include <cmath>

EnvelopeFollower::EnvelopeFollower(float attackTime, float releaseTime, float sampleRate) 
    : attackTime(attackTime), releaseTime(releaseTime), sampleRate(sampleRate) {
    fifoBuffer.resize(fifo.getTotalSize());
    writeToFifo(0.0f);
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

void EnvelopeFollower::updateCoefficients() {
    attackCoef  = std::exp(-1.0f / (attackTime * sampleRate));
    releaseCoef = std::exp(-1.0f / (releaseTime * sampleRate));
}
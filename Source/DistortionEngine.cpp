#include "DistortionEngine.h"

DistortionEngine::DistortionEngine() : drive(1.0f), distortionAlgorithm(0) {

}

void DistortionEngine::setDistortionAlgorithm(int algorithm) {
    distortionAlgorithm = algorithm;
}

void DistortionEngine::setDrive(float newDrive) {
    drive = newDrive;
}

void DistortionEngine::setModulation(float newModulation) {
    modulation = newModulation;
}

float DistortionEngine::getDrive() {
    return drive + (modulation * 20.0f);
}

std::vector<float> DistortionEngine::getWaveshape() {
    std::vector<float> waveshape;
    waveshape.reserve(64);

    for (int i = 0; i < 64; ++i) {
        float input = juce::jmap(static_cast<float>(i), 0.0f, 63.0f, -1.0f, 1.0f);
        waveshape.push_back(distort(input));
    }

    return waveshape;
}

float DistortionEngine::processSample(float sample) {
    return distort(sample);
}

float sign(float x) {
    if (x >= 0) return 1.0;
    return -1.0;
}

float DistortionEngine::hardClip(float x) {
    return juce::jlimit(-1.0f, 1.0f, x * (getDrive() + 1.0f));
}

float DistortionEngine::tube(float x) {
    float _drive = getDrive();
    return std::tanh(x * _drive) / std::tanh(_drive);
}

float DistortionEngine::fuzz(float x) {
    float _drive = getDrive();
    return sign(x) * ((1 - pow(M_E, -abs(_drive * x))) / (1 - pow(M_E, -_drive)));
}

float DistortionEngine::rectify(float x) {
    return hardClip(abs(x));
}

float DistortionEngine::downsample(float x) {
    int numSteps = std::round(64.0f - (getDrive() / 20.0f) * 60.0f); // Get the number of steps based on drive (from 64 to 4)
    numSteps = std::max(numSteps, 4); // Ensure it doesn't go below 4 steps

    return std::round(x * numSteps) / numSteps;
}

float DistortionEngine::distort(float sample) {
    switch (distortionAlgorithm)
    {
    case 0:
        return hardClip(sample);
    case 1:
        return tube(sample);
    case 2:
        return fuzz(sample);
    case 3:
        return rectify(sample);
    case 4:
        return downsample(sample);
    }
}
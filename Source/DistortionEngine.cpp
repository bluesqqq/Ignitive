#include "DistortionEngine.h"

DistortionEngine::DistortionEngine() : drive(1.0f), type(DistortionType::HardClip) { }

void DistortionEngine::setDistortionAlgorithm(DistortionType distType) { type = distType; }

void DistortionEngine::setDrive(float newDrive) { drive = std::clamp(newDrive, 0.0f, 20.0f); }

float DistortionEngine::getDrive() { return drive; }

std::vector<float> DistortionEngine::getWaveshape() {
    std::vector<float> waveshape;
    waveshape.reserve(64);

    for (int i = 0; i < 64; ++i) {
        float input = juce::jmap(static_cast<float>(i), 0.0f, 63.0f, -1.0f, 1.0f);
        waveshape.push_back(distort(input));
    }

    return waveshape;
}

float DistortionEngine::processSample(float sample) { return distort(sample); }

float sign(float x) {
    if (x >= 0) return 1.0;
    return -1.0;
}

float DistortionEngine::hardClip(float x) { return juce::jlimit(-1.0f, 1.0f, x * (drive + 1.0f)); }

float DistortionEngine::tube(float x) {
    if (drive == 0.0f) return x;
    return std::tanh(x * drive) / std::tanh(drive);
}

float DistortionEngine::fuzz(float x) { return sign(x) * ((1 - pow(M_E, -abs(drive * x))) / (1 - pow(M_E, -drive))); }

float DistortionEngine::rectify(float x) { return hardClip(abs(x)); }

float DistortionEngine::downsample(float x) {
    int numSteps = std::round(64.0f - (getDrive() / 20.0f) * 60.0f); // Get the number of steps based on drive (from 64 to 4)
    numSteps = std::max(numSteps, 4); // Ensure it doesn't go below 4 steps

    return std::round(x * numSteps) / numSteps;
}

float DistortionEngine::distort(float sample) {
    switch (type) {
        case DistortionType::HardClip:   return hardClip(sample);
        case DistortionType::Tube:       return tube(sample);
        case DistortionType::Fuzz:       return fuzz(sample);
        case DistortionType::Rectify:    return rectify(sample);
        case DistortionType::Downsample: return downsample(sample);
    }

    return sample;
}
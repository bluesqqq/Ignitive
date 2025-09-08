#include "DistortionProcessor.h"

DistortionProcessor::DistortionProcessor(juce::AudioProcessorValueTreeState& params, const juce::String& driveID, const juce::String& colorID, const juce::String& typeID)
    : drive(0.0f), type(DistortionType::HardClip), parameters(params),
      driveID(driveID), colorID(colorID), typeID(typeID) {
}

void DistortionProcessor::prepare(const juce::dsp::ProcessSpec& spec) {
    oversampler = std::make_unique<juce::dsp::Oversampling<float>>(
        spec.numChannels,
        2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR
    );

    oversampler->initProcessing(spec.maximumBlockSize);

    drive.reset(spec.sampleRate, 0.02);
    color.reset(spec.sampleRate, 0.02);
}

void DistortionProcessor::process(const juce::dsp::ProcessContextReplacing<float>& context) {
    updateParameters();

    auto& inBlock  = context.getInputBlock();
    auto& outBlock = context.getOutputBlock();

    // Upsample
    auto oversampledBlock = oversampler->processSamplesUp(inBlock);

    // Process
    for (size_t sample = 0; sample < oversampledBlock.getNumSamples(); ++sample) {
        float d = drive.getNextValue();
        float c = color.getNextValue();

        for (size_t channel = 0; channel < oversampledBlock.getNumChannels(); ++channel) {
            auto* samples = oversampledBlock.getChannelPointer(channel);
            samples[sample] = distort(samples[sample], d, c);
        }
    }

    // Downsample
    auto oversampledContext = juce::dsp::ProcessContextReplacing<float>(oversampledBlock);
    
    oversampler->processSamplesDown(outBlock);
}

void DistortionProcessor::reset() {
}

void DistortionProcessor::setDistortionAlgorithm(DistortionType distType) { type = distType; }

void DistortionProcessor::updateParameters() {
    drive.setTargetValue(*parameters.getRawParameterValue(driveID));
    color.setTargetValue(*parameters.getRawParameterValue(colorID));

    int distortionType = parameters.getRawParameterValue(typeID)->load();

    setDistortionAlgorithm(static_cast<DistortionType>(distortionType));
}

std::vector<float> DistortionProcessor::getWaveshape() {
    float d = *parameters.getRawParameterValue(driveID);
    float c = *parameters.getRawParameterValue(colorID);
    std::vector<float> waveshape;
    waveshape.reserve(64);

    for (int i = 0; i < 64; ++i) {
        float input = juce::jmap(static_cast<float>(i), 0.0f, 63.0f, -1.0f, 1.0f);
        waveshape.push_back(distort(input, d, c));
    }

    return waveshape;
}

float sign(float x) {
    if (x >= 0) return 1.0;
    return -1.0;
}

float DistortionProcessor::hardClip(float x, float d) { return juce::jlimit(-1.0f, 1.0f, x * (1.0f + d * 20.0f)); }

float DistortionProcessor::tube(float x, float d) {
    if (d == 0.0f) return x;
    float k = d * 20.0f;
    return std::tanf(x * k) / std::tanhf(k);
}

float DistortionProcessor::fuzz(float x, float d) {
    float k = d * 20.0f;
    return sign(x) * ((1 - std::expf(-std::fabsf(k * x))) / (1 - std::expf(-k))); 
}

float DistortionProcessor::rectify(float x, float d) { return hardClip(fabsf(x), d); }

float DistortionProcessor::downsample(float x, float d) {
    int numSteps = juce::jmax(4, (int)std::lround(64.0f - d * 60.0f)); // Get the number of steps based on drive (from 64 to 4)
    return std::floor(x * numSteps + 0.5f) / numSteps;
}

float DistortionProcessor::distort(float x, float d, float c) {
    const float minDrive = 0.5f;
    const float maxDrive = 1.2f;

    float tilt = (c - 0.5f) * 2.0f;

    float side = (x >= 0.0f ? 1.0f : -1.0f);

    float driveFactor = juce::jmap(tilt * side,
        -1.0f, 1.0f,
        minDrive, maxDrive);

    d = d * driveFactor;

    switch (type) {
        case DistortionType::HardClip:   return hardClip(x, d);
        case DistortionType::Tube:       return tube(x, d);
        case DistortionType::Fuzz:       return fuzz(x, d);
        case DistortionType::Rectify:    return rectify(x, d);
        case DistortionType::Downsample: return downsample(x, d);
    }

    return x;
}
#include "DistortionProcessor.h"
#include "Parameters.h"

DistortionProcessor::DistortionProcessor(juce::AudioProcessorValueTreeState& params)
    : drive(1.0f), type(DistortionType::HardClip), parameters(params) {
}

void DistortionProcessor::prepare(const juce::dsp::ProcessSpec& spec) {
    oversampler = std::make_unique<juce::dsp::Oversampling<float>>(
        spec.numChannels,
        2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR
    );

    oversampler->initProcessing(spec.maximumBlockSize);
}

void DistortionProcessor::process(const juce::dsp::ProcessContextReplacing<float>& context) {
    float drive = parameters.getRawParameterValue(Parameters::ID_DRIVE)->load();
    float color = parameters.getRawParameterValue(Parameters::ID_COLOR)->load();
    int distortionType = parameters.getRawParameterValue(Parameters::ID_DISTORTION_TYPE)->load();

    setDrive(drive);
    setDistortionAlgorithm(static_cast<DistortionType>(distortionType));

    auto& inBlock  = context.getInputBlock();
    auto& outBlock = context.getOutputBlock();

    // Upsample
    auto oversampledBlock = oversampler->processSamplesUp(inBlock);

    // Process
    for (size_t channel = 0; channel < oversampledBlock.getNumChannels(); ++channel) {
        auto* samples = oversampledBlock.getChannelPointer(channel);
        for (size_t n = 0; n < oversampledBlock.getNumSamples(); ++n)
            samples[n] = distort(samples[n]);
    }

    // Downsample
    auto oversampledContext = juce::dsp::ProcessContextReplacing<float>(oversampledBlock);
    
    oversampler->processSamplesDown(outBlock);
}

void DistortionProcessor::reset() {
}




void DistortionProcessor::setDistortionAlgorithm(DistortionType distType) { type = distType; }

void DistortionProcessor::setDrive(float newDrive) { drive = std::clamp(newDrive, 0.0f, 20.0f); }

float DistortionProcessor::getDrive() { return drive; }

std::vector<float> DistortionProcessor::getWaveshape() {
    std::vector<float> waveshape;
    waveshape.reserve(64);

    for (int i = 0; i < 64; ++i) {
        float input = juce::jmap(static_cast<float>(i), 0.0f, 63.0f, -1.0f, 1.0f);
        waveshape.push_back(distort(input));
    }

    return waveshape;
}

float DistortionProcessor::processSample(float sample) { return distort(sample); }

float sign(float x) {
    if (x >= 0) return 1.0;
    return -1.0;
}

float DistortionProcessor::hardClip(float x) { return juce::jlimit(-1.0f, 1.0f, x * (1.0f + drive * 20.0f)); }

float DistortionProcessor::tube(float x) {
    if (drive == 0.0f) return x;
    float k = drive * 20.0f;
    return std::tanf(x * k) / std::tanhf(k);
}

float DistortionProcessor::fuzz(float x) { 
    float k = drive * 20.0f;
    return sign(x) * ((1 - std::expf(-std::fabsf(k * x))) / (1 - std::expf(-k))); 
}

float DistortionProcessor::rectify(float x) { return hardClip(fabsf(x)); }

float DistortionProcessor::downsample(float x) {
    int numSteps = juce::jmax(4, (int)std::lround(64.0f - drive * 60.0f)); // Get the number of steps based on drive (from 64 to 4)
    return std::floor(x * numSteps + 0.5f) / numSteps;
}

float DistortionProcessor::distort(float sample) {
    switch (type) {
        case DistortionType::HardClip:   return hardClip(sample);
        case DistortionType::Tube:       return tube(sample);
        case DistortionType::Fuzz:       return fuzz(sample);
        case DistortionType::Rectify:    return rectify(sample);
        case DistortionType::Downsample: return downsample(sample);
    }

    return sample;
}
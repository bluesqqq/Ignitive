#include "DistortionProcessor.h"

const std::array<DistortionDefinition, 7> DistortionProcessor::distortionDefs = { {
    {
        "Hard Clip",
        [](float sample, float drive) -> float {
            return juce::jlimit(-1.0f, 1.0f, sample * (1.0f + drive * 20.0f));
        }
    },
    {
        "Tube",
        [](float sample, float drive) -> float {
            if (drive <= 0.0f) return sample;
            float k = drive * 20.0f;
            return std::tanhf(sample * k) / std::tanhf(k);
        }
    },
    {
        "Tape",
        [](float sample, float drive) -> float {
            if (drive <= 0.0f) return sample;

            float k = 1.0f + drive * 5.0f;
            float x = sample * k;

            return (x / (1.0f + fabsf(x)));
        }
    },
    {
        "LinearShaper",
        [](float sample, float drive) -> float {
            float k = 1.0f + drive * 15.0f;

            return 1.0f - std::fabsf(std::fmodf(sample * k + 1.0f, 4.0f) - 2.0f);
        }
    },
    {
        "SineShaper",
        [](float sample, float drive) -> float {
            float k = 1.0f + drive * 15.0f;

            return sin(sample * k);
        }
    },
    {
        "Rectify",
        [](float sample, float drive) -> float {
            return juce::jlimit(-1.0f, 1.0f, fabsf(sample) * (1.0f + drive * 20.0f));
        }
    },
    {
        "Downsample",
        [](float sample, float drive) -> float {
            int numSteps = juce::jmax(2, (int)std::lround(64.0f - drive * 62.0f)); // Get the number of steps based on drive (from 64 to 4)
            return std::floor(sample * numSteps + 0.5f) / numSteps;
        }
    },
}};

DistortionProcessor::DistortionProcessor(juce::AudioProcessorValueTreeState& params, ModMatrix& matrix, const juce::String& driveID, const juce::String& colorID, const juce::String& typeID)
    : parameters(params), modMatrix(matrix),
      driveID(driveID), colorID(colorID), typeID(typeID),
      index(0) {
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
    updateParameters();

    auto& inBlock  = context.getInputBlock();
    auto& outBlock = context.getOutputBlock();

    // Upsample
    auto oversampledBlock = oversampler->processSamplesUp(inBlock);

    // Process
    for (size_t sample = 0; sample < oversampledBlock.getNumSamples(); ++sample) {
        float d = modMatrix.getValue(Parameters::ID_DRIVE, sample/4);
        float c = modMatrix.getValue(Parameters::ID_COLOR, sample/4);

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

void DistortionProcessor::setDistortionAlgorithm(DistortionType distType) { index = static_cast<int>(distType); }

void DistortionProcessor::updateParameters() {
    int distortionType = parameters.getRawParameterValue(typeID)->load();

    setDistortionAlgorithm(static_cast<DistortionType>(distortionType));
}

std::vector<float> DistortionProcessor::getWaveshape() {
    float d = modMatrix.getValue(driveID, 0);
    float c = modMatrix.getValue(colorID, 0);
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

float DistortionProcessor::distort(float x, float d, float c) {
    auto& def = distortionDefs[index];

    float distortion = d;

    // BEND
    //float distortion = d + c * (1.0f - std::fabsf(x));

    // ASYM
    //float offset = (c - 0.5f) * 1.5f;
    //if (x <= offset) distortion += (1.0f / (offset + 1.0f)) * (x + 1.0f);
    //else distortion += (1.0f / (offset - 1.0f)) * (x - 1.0f);

    /*
    const float minDrive = 0.5f;
    const float maxDrive = 1.2f;
    float tilt = (c - 0.5f) * 2.0f;

    float side = (x >= 0.0f ? 1.0f : -1.0f);

    float driveFactor = juce::jmap(tilt * side, -1.0f, 1.0f, minDrive, maxDrive);
    */

    distortion = juce::jlimit(0.0f, 2.0f, distortion);

    x = def.process(x, distortion);

    return x;
}
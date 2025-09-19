#include "DistortionProcessor.h"

const std::array<DistortionDefinition, 4> DistortionProcessor::distortionDefs = { {
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
        "Downsample",
        [](float sample, float drive) -> float {
            int numSteps = juce::jmax(2, (int)std::lround(64.0f - drive * 62.0f)); // Get the number of steps based on drive (from 64 to 4)
            return std::floor(sample * numSteps + 0.5f) / numSteps;
        }
    },
}};

const std::array<juce::String, 4> DistortionProcessor::characterDefs = {
    "Bend",
    "Asym",
    "Fold",
    "Rectify"
};

DistortionProcessor::DistortionProcessor(juce::AudioProcessorValueTreeState& params, ModMatrix& matrix, const juce::String& driveID, const juce::String& characterID, const juce::String& typeID, const juce::String& characterTypeID)
    : parameters(params), modMatrix(matrix),
      driveID(driveID), characterID(characterID), typeID(typeID), characterTypeID(characterTypeID),
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
        float c = modMatrix.getValue(Parameters::ID_CHARACTER, sample/4);

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

void DistortionProcessor::updateParameters() {
    int distortionType = parameters.getRawParameterValue(typeID)->load();
    characterType = static_cast<CharacterType>(parameters.getRawParameterValue(characterTypeID)->load());

    index = distortionType;
}

std::vector<float> DistortionProcessor::getWaveshape(unsigned int points) {
    float d = modMatrix.getValue(driveID, 0);
    float c = modMatrix.getValue(characterID, 0);
    std::vector<float> waveshape;
    waveshape.reserve(points);

    for (int i = 0; i < points; ++i) {
        float input = juce::jmap(static_cast<float>(i), 0.0f, static_cast<float>(points - 1), -1.0f, 1.0f);
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

    switch (characterType) {
        case CharacterType::Bend: {
            distortion += fabsf(x * c * 2.0f) - c;
            break;
        }
        case CharacterType::Asym: {
            distortion += x * c;
            break;
        }
        case CharacterType::Fold: {
            x *= 1.0f + c * 8.0f;
            float xm = x - 1.0f;
            xm = xm - 4.0f * std::floor(xm * 0.25f); // faster than fmod for positive mod

            x = std::fabs(xm - 2.0f) - 1.0f;
            break;
        }
        case CharacterType::Rectify: {
            x = (1 - c) * x + c * std::fabsf(x);
            break;
        }
    }
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
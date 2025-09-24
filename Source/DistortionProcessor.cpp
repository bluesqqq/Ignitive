#include "DistortionProcessor.h"

const std::vector<DistortionDefinition> DistortionProcessor::distortionDefs = { {
    {
        "Hard Clip",
        [](float sample) -> float {
            return sample;
        }
    },
    {
        "Soft Clip",
        [](float sample) -> float {
            return std::tanhf(sample);
        }
    },
    {
        "Tube",
        [](float sample) -> float {
            return sample;
        }
    },
    {
        "Test 1",
        [](float sample) -> float {
            if (sample < -0.5)
                return (sample + 0.5) * 2;
            else if (sample <= 0.5)
                return (std::sinf(sample * juce::MathConstants<float>::twoPi * 4) * 0.3);
           
            return std::atanf(sample - 0.5) * 1.5f;
        }
    },
    {
        "Test 2",
        [](float sample) -> float {
            if (sample < -0)
                return -std::fabsf(2 * sample + 1) + 1;
            
            return sample;
        }
    },
    {
        "Test 3",
        [](float sample) -> float {
            float absSample = std::fabsf(sample);
            return absSample * std::sinf((sample / (1.0f + 0.05f * absSample)) * juce::MathConstants<float>::halfPi);
        }
    },
    {
        "Test 4",
        [](float sample) -> float {
            if (sample < -10) return sin(sample * juce::MathConstants<float>::pi) - 20;
            else if (sample < 0.25) return sample * 2;
            else if (sample <= 4) return 0.5;

            return (sample - 4) * (sample - 15) * 0.3 + 0.5;
        }
    },
    {
        "Test 5",
        [](float sample) -> float {
            return (20 / juce::MathConstants<float>::pi) * std::atanf(0.2f * sample) + std::sinf(std::powf(std::fabsf(sample / 3.0f), 2.0f));
        }
    },
    {
        "Test 6",
        [](float sample) -> float {
            if (sample < 0) return std::min(0.0f, std::sinf(sample * juce::MathConstants<float>::pi) * sample);
            
            return sample;
        }
    },
    {
        "Test 7",
        [](float sample) -> float {
            if (sample < 0) {
                float c = sample + 4;
                
                return juce::jlimit(sample, -sample, sample * (sample + 2) * c * c * c);
            }

            float a = std::powf(sample, 1.6) - 1.0f;
            float b = sample + 1.0f;
            return a - b * std::floor(a / b);
        }
    },
    {
        "Test 8",
        [](float sample) -> float {
            return sample + std::ceilf(std::fmodf(sample, 1.0f) - 0.5f) * sample * 0.3f;
        }
    },
    {
        "Test 9",
        [](float sample) -> float {
            return juce::jlimit((sample - 20.0f) / 20.0f, (sample + 20.0f) / 20.0f, sample);
        }
    },
    {
        "Test 10",
        [](float sample) -> float {
            return 0.05f * sample * std::abs(sample) + 0.5f * std::sin(0.9f * sample);
        }
    }
}};

const std::array<juce::String, 7> DistortionProcessor::characterDefs = {
    "Bend+",
    "Bend-",
    "Asym+",
    "Asym-",
    "Fold",
    "Rectify",
    "Bitcrush"
};

DistortionProcessor::DistortionProcessor(juce::AudioProcessorValueTreeState& params, ModMatrix& matrix, const juce::String& driveID, const juce::String& characterID, const juce::String& typeID, const juce::String& characterTypeID, const juce::String& oversampleID)
    : parameters(params), modMatrix(matrix),
      driveID(driveID), characterID(characterID), typeID(typeID), characterTypeID(characterTypeID), oversampleID(oversampleID),
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

    auto& inBlock = context.getInputBlock();
    auto& outBlock = context.getOutputBlock();

    if (oversample) {
        auto oversampledBlock = oversampler->processSamplesUp(inBlock);

        for (size_t sample = 0; sample < oversampledBlock.getNumSamples(); ++sample) {
            float d = modMatrix.getValue(Parameters::ID_DRIVE, sample / 4);
            float c = modMatrix.getValue(Parameters::ID_CHARACTER, sample / 4);

            for (size_t channel = 0; channel < oversampledBlock.getNumChannels(); ++channel) {
                auto* samples = oversampledBlock.getChannelPointer(channel);
                samples[sample] = distort(samples[sample], d, c);
            }
        }

        oversampler->processSamplesDown(outBlock);
    } else {
        for (size_t sample = 0; sample < outBlock.getNumSamples(); ++sample) {
            float d = modMatrix.getValue(Parameters::ID_DRIVE, sample);
            float c = modMatrix.getValue(Parameters::ID_CHARACTER, sample);

            for (size_t channel = 0; channel < outBlock.getNumChannels(); ++channel) {
                auto* samples = outBlock.getChannelPointer(channel);
                samples[sample] = distort(samples[sample], d, c);
            }
        }
    }
}


void DistortionProcessor::reset() {
}

void DistortionProcessor::updateParameters() {
    index = parameters.getRawParameterValue(typeID)->load();
    characterType = static_cast<CharacterType>(parameters.getRawParameterValue(characterTypeID)->load());
    oversample = parameters.getRawParameterValue(oversampleID)->load();
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

float DistortionProcessor::distort(float sample, float drive, float character) {
    auto& def = distortionDefs[index];

    drive = 1 + drive * 19; // Gets drive into (1-20) range

    // Distortion Modifier (Character)
    switch (characterType) {
        case CharacterType::BendPlus: { // More drive near -1, 1, less near 0
            drive *= fabsf(sample * character * 2.0f) + 1 - character;
            break;
        }
        case CharacterType::BendMinus: { // More drive near 0, less near -1, 1
            drive *= -fabsf(sample * character * 2.0f) + 1 + character;
            break;
        }
        case CharacterType::AsymPositive: { // More drive near 1, less near -1
            drive *= sample * character + 1;
            break;
        }
        case CharacterType::AsymNegative: { // More drive near -1, less near 1
            drive *= -sample * character + 1;
            break;
        }
        case CharacterType::Fold: {
            sample *= 1.0f + character * 8.0f;
            float xm = sample - 1.0f;
            xm = xm - 4.0f * std::floor(xm * 0.25f);

            sample = std::fabs(xm - 2.0f) - 1.0f;
            break;
        }
        case CharacterType::Rectify: {
            sample = (1 - character) * sample + character * std::fabsf(sample);
            break;
        }
    }

    // Apply drive then distort
    sample *= drive;
    sample = def.process(sample);

    if (characterType == CharacterType::Bitcrush) {
        float steps = juce::jmap(character, 64.0f, 2.0f);

        sample = std::round(sample * steps) / steps;
    }

    return sample;
}
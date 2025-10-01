#include "DistortionProcessor.h"
#include <random>

const void DistortionProcessor::makeDistortionAlgosMenu(juce::PopupMenu& menu) {
    std::unordered_map<DistortionCategory, juce::PopupMenu> categorySubmenus;
    int itemID = 1;

    for (const auto& algo : distortionAlgos) {
		auto& submenu = categorySubmenus[algo.category];
		submenu.addItem(itemID++, algo.name);
    }

    for (const auto& categoryPair : categorySubmenus)
		menu.addSubMenu(categoryNames[static_cast<int>(categoryPair.first)], categoryPair.second);
}

DistortionProcessor::DistortionProcessor(juce::AudioProcessorValueTreeState& params, ModMatrix& matrix, const juce::String& driveID, const juce::String& characterID, const juce::String& typeID, const juce::String& characterTypeID, const juce::String& characterPolarityID, const juce::String& oversampleID)
    : parameters(params), modMatrix(matrix),
      driveID(driveID), characterID(characterID), typeID(typeID), characterTypeID(characterTypeID), characterPolarityID(characterPolarityID), oversampleID(oversampleID),
      currentAlgorithm(getDistortionAlgorithm()), currentCharacterAlgo(getCharacterAlgorithm()) {
}

void DistortionProcessor::prepare(const juce::dsp::ProcessSpec& spec) {
    oversampler = std::make_unique<juce::dsp::Oversampling<float>>(spec.numChannels, 2, juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR);
    oversampler->initProcessing(spec.maximumBlockSize);
}

void DistortionProcessor::process(const juce::dsp::ProcessContextReplacing<float>& context) {
    updateParameters();

    auto& inBlock = context.getInputBlock();
    auto& outBlock = context.getOutputBlock();

    ModDestination* drive     = modMatrix.getDestination(Parameters::ID_DRIVE);
    ModDestination* character = modMatrix.getDestination(Parameters::ID_CHARACTER);

    if (oversample) {
        auto oversampledBlock = oversampler->processSamplesUp(inBlock);

        for (size_t sample = 0; sample < oversampledBlock.getNumSamples(); ++sample) {
            float d = drive->getValue(sample / 4);
            float c = character->getValue(sample / 4);

            for (size_t channel = 0; channel < oversampledBlock.getNumChannels(); ++channel) {
                auto* samples = oversampledBlock.getChannelPointer(channel);
                samples[sample] = distort(samples[sample], d, c);
            }
        }

        oversampler->processSamplesDown(outBlock);
    } else {
        for (size_t sample = 0; sample < outBlock.getNumSamples(); ++sample) {
            float d = drive->getValue(sample / 4);
            float c = character->getValue(sample / 4);

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
    auto* param = dynamic_cast<juce::AudioParameterChoice*>(parameters.getParameter(typeID));

    if (param != nullptr) {
        distortionIndex = param->getIndex();
        currentAlgorithm = getDistortionAlgorithm();
    }

    characterIndex = parameters.getRawParameterValue(characterTypeID)->load();
	currentCharacterAlgo = getCharacterAlgorithm();
    characterPolarity = static_cast<CharacterPolarity>(parameters.getRawParameterValue(characterPolarityID)->load());

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
    drive = 1 + drive * 19; // Gets drive into (1-20) range

	if (currentCharacterAlgo->polarized) { // Polarize value if needed
        switch (characterPolarity) {
            case CharacterPolarity::Minus:     character = -character; break;
            case CharacterPolarity::PlusMinus: character = (character * 2.0f) - 1.0f; break;
        }
    }

    currentCharacterAlgo->process(sample, drive, character);

    // Apply drive then distort
    sample *= drive;
    sample = currentAlgorithm->process(sample);

    return sample;
}

// ================/ DISTORTION ALGORITHMS /================
float whiteNoise(float min, float max) {
    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

float pinkNoise(float min, float max) {
    static std::mt19937 gen(12345);
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    static float b0 = 0, b1 = 0, b2 = 0, b3 = 0, b4 = 0, b5 = 0, b6 = 0;

    float white = dist(gen);
    b0 = 0.99886f * b0 + white * 0.0555179f;
    b1 = 0.99332f * b1 + white * 0.0750759f;
    b2 = 0.96900f * b2 + white * 0.1538520f;
    b3 = 0.86650f * b3 + white * 0.3104856f;
    b4 = 0.55000f * b4 + white * 0.5329522f;
    b5 = -0.7616f * b5 - white * 0.0168980f;
    float pink = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362f;
    b6 = white * 0.115926f;

    return min + (pink + 3.0f) * 0.1666667f * (max - min);
}

float blueNoise(float min, float max) {
    static std::mt19937 gen(12345);
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    static float last = 0.5f;

    float current = dist(gen);
    float blue = current - last; // differentiates white noise
    last = current;

    // simple scaling to desired range (blue is roughly [-1,1])
    return min + (blue + 0.5f) * (max - min);
}

const std::vector<DistortionAlgorithm> DistortionProcessor::distortionAlgos = {
    // V1.0.0
    { "Hard Clip", "Cuts peaks sharply, creating harsh distortion", DistortionCategory::Classic,
        [](float sample) -> float {
            return juce::jlimit(-1.0f, 1.0f, sample);
        }
    },
    { "Tube", "Smooth, warm saturation with soft compression.", DistortionCategory::Classic,
        [](float sample) -> float {
            return std::tanhf(sample);
        }
    },
    { "Overdrive", "Adds crunch, emulating a stompbox.", DistortionCategory::Classic,
        [](float sample) -> float {
            if (sample < -1.0f) return 0.05f * sample - 0.95f;
            else if (sample <= 1.0f) return sample;
            return 0.05f * sample + 0.95f;
        }
    },
    { "Clippy", "Softer, smoother version of Overdrive", DistortionCategory::Classic,
        [](float sample) -> float {
            return std::tanhf(sample) + sample * 0.05f;
        }
    },
    { "Breakdown", "Buzzy at high drive", DistortionCategory::Digital,
        [](float sample) -> float {
            float y = sample * sample * sample * 0.003f;
            if (y == 0.0f) return sample;
            return sample + (sample - y * std::floor(sample / y));
        }
    },
    { "Nails", "Adds sharp spiky character.", DistortionCategory::Digital,
        [](float sample) -> float {
            float mod2 = sample - 2.0f * std::floor(sample / 2.0f);
            float absVal = std::fabs(mod2 - 1.0f);
            float result = absVal * 10.0f + sample - 2.0f;
            return std::min(sample, result);
        }
    },
    { "Stepper", "NO DESCRIPTION", DistortionCategory::Digital,
        [](float sample) -> float {
            return sample + std::ceilf(std::fmodf(sample, 1.0f) - 0.5f) * sample * 0.3f;
        }
    },
    { "Tooth", "Sharp, asymmetrical, almost rectifying.", DistortionCategory::Digital,
        [](float sample) -> float {
            if (sample < 0)
                return -std::fabsf(2 * sample + 1) + 1;
            return sample;
        }
    },
    { "Hazy Signal", "Noisy signal increases with drive.", DistortionCategory::Broken,
        [](float sample) -> float {
            static thread_local std::mt19937 gen(std::random_device{}());
            float noiseAmount = std::pow(std::fabs(sample), 2) * 0.02f;
            std::uniform_real_distribution<float> dist(-noiseAmount, noiseAmount);
            return sample + dist(gen);
        }
    },
    { "Blown Tube", "Broken tube saturator with noisy fuzz.", DistortionCategory::Broken,
        [](float sample) -> float {
            static thread_local std::mt19937 gen(std::random_device{}());
            float noiseAmount = std::max(0.0f, std::fabsf(sample) - 2) * 0.01f;
            std::uniform_real_distribution<float> dist(-noiseAmount, noiseAmount);
            return std::tanhf(sample) + dist(gen);
        }
    },
    { "Wavebreaker", "Motion of the ocean.", DistortionCategory::Formant,
        [](float sample) -> float {
            float absSample = std::fabsf(sample);
            return absSample * std::sinf((sample / (1.0f + 0.05f * absSample)) * juce::MathConstants<float>::halfPi);
        }
    },
    { "Vowel Morph", "Vocal-like harmonics.", DistortionCategory::Formant,
        [](float sample) -> float {
            return (20 / juce::MathConstants<float>::pi) * std::atanf(0.2f * sample)
                + std::sinf(std::powf(std::fabsf(sample / 3.0f), 2.0f));
        }
    },
    { "Voice Box", "Spoken like a true distortion algorithm.", DistortionCategory::Formant,
        [](float sample) -> float {
            return 0.05f * sample * std::abs(sample) + 0.5f * std::sin(0.9f * sample);
        }
    },
    { "Skinwalker", "Vowel-y shape that turns into hum.", DistortionCategory::Formant,
        [](float sample) -> float {
            return sample + 0.4f * std::sinf(10.0f * (std::log(std::fabs(sample * 0.1f) + 0.1f) + 1.0f));
        }
    },
    { "Analog Buzz", "Analog shape that fizzes out into a buzz.", DistortionCategory::Experimental,
        [](float sample) -> float {
            if (sample < -0.5)
                return (sample + 0.5f) * 2.0f;
            else if (sample <= 0.5)
                return std::sinf(sample * juce::MathConstants<float>::twoPi * 4) * 0.3f;
            return std::atanf(sample - 0.5f) * 1.5f;
        }
    },
    { "Mouth Breather", "NO DESCRIPTION", DistortionCategory::Experimental,
        [](float sample) -> float {
            if (sample < -10) return std::sin(sample * juce::MathConstants<float>::pi) - 20;
            else if (sample < 0.25) return sample * 2;
            else if (sample <= 4) return 0.5f;
            return (sample - 4) * (sample - 15) * 0.3f + 0.5f;
        }
    },
    { "Sinegate", "Chopped sine wave creates harsh, screaming tone.", DistortionCategory::Experimental,
        [](float sample) -> float {
            if (sample < 0) return std::min(0.0f, std::sinf(sample * juce::MathConstants<float>::pi) * sample);
            return sample;
        }
    },
    { "Fractal", "Sharp, aggresive, evolving tones.", DistortionCategory::Experimental,
        [](float sample) -> float {
            if (sample < 0) {
                float c = sample + 4;
                return juce::jlimit(sample, -sample, sample * (sample + 2) * c * c * c);
            }
            float a = std::powf(sample, 1.6f) - 1.0f;
            float b = sample + 1.0f;
            return a - b * std::floor(a / b);
        }
    }
    /*
        If new algorithms are added, they must be added here at the end.

        Algorithm selection is stored as an index in the parameter tree, so any changes to the order will affect
        presets from previous versions.
    */
};

// ================/ CHARACTER ALGORITHMS /================
const std::vector<CharacterAlgorithm> DistortionProcessor::characterAlgos{
    { "Bend", "Increases drive near center/edges", true,
        [](float& sample, float& drive, float& character) -> void {
            drive *= character * std::fabsf(sample * character * 2.0f) + 1.0f - character;
        }
    },
    { "Asym", "Shifts drive asymmetrically to one side", true,
        [](float& sample, float& drive, float& character) -> void {
            drive *= sample * character + 1.0f;
        }
    },
    { "Fold", "", false,
        [](float& sample, float& drive, float& character) -> void {
            sample *= 1.0f + character * 8.0f;
            float xm = sample - 1.0f;
            xm = xm - 4.0f * std::floor(xm * 0.25f);

            sample = std::fabs(xm - 2.0f) - 1.0f;
        }
    },
    { "Rectify", "", true,
        [](float& sample, float& drive, float& character) -> void {
            if (character > 0.0f) {
                sample = std::fabsf(sample) * character + sample * (1.0f - character);
            } else if (character < 0.0f) {
                    sample = -std::fabsf(sample) * std::fabsf(character) + sample * (1.0f - std::fabsf(character));
            }
        }
    },
    { "Quantize", "", false,
        [](float& sample, float& drive, float& character) -> void {
            float steps = juce::jmap(character, 64.0f, 2.0f);

            sample = std::round(sample * steps) / steps;
        }
    }
};
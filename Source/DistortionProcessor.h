#pragma once

#define _USE_MATH_DEFINES

#include <vector>
#include <JuceHeader.h>
#include <cmath>
#include "ModMatrix.h"

enum class DistortionType {
	HardClip,
	Tube,
	Tape,
	Overdrive,
	NumDefinitions
};

enum class CharacterType {
	Bend,
	Asym,
	Fold,
	Rectify,
	Bitcrush
};

struct DistortionDefinition {
	const char* name;
	float (*process)(float, float);
};

class DistortionProcessor : public juce::dsp::ProcessorBase {
	private:
		juce::AudioProcessorValueTreeState& parameters;
		ModMatrix& modMatrix;

		std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;

		float distort(float sample, float drive, float character);

		int index;

		CharacterType characterType = CharacterType::Asym;

		juce::String driveID;
		juce::String characterID;
		juce::String typeID;
		juce::String characterTypeID;

	public:
		static const std::array<DistortionDefinition, 4> distortionDefs;
		static const std::array<juce::String, 5> characterDefs;

		DistortionProcessor(juce::AudioProcessorValueTreeState& params, ModMatrix& modMatrix, const juce::String& driveID, const juce::String& characterID, const juce::String& typeID, const juce::String& characterTypeID);

		void prepare(const juce::dsp::ProcessSpec& spec) override;
		void process(const juce::dsp::ProcessContextReplacing<float>& context) override;
		void reset() override;

		void updateParameters();

		std::vector<float> getWaveshape(unsigned int points = 64);

		float getModifiedDriveValue() {
			return modMatrix.getValue(driveID, 0);
		}

		float getModifiedCharacterValue() {
			return modMatrix.getValue(characterID, 0);
		}
};
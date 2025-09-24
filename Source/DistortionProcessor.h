#pragma once

#define _USE_MATH_DEFINES

#include <vector>
#include <JuceHeader.h>
#include <cmath>
#include "ModMatrix.h"

enum class CharacterType {
	BendPlus,
	BendMinus,
	AsymPositive,
	AsymNegative,
	Fold,
	Rectify,
	Bitcrush
};

struct DistortionDefinition {
	const char* name;
	float (*process)(float);
};

class DistortionProcessor : public juce::dsp::ProcessorBase {
	private:
		juce::AudioProcessorValueTreeState& parameters;
		ModMatrix& modMatrix;
		std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;

		// Parameter IDs
		juce::String driveID;
		juce::String characterID;
		juce::String typeID;
		juce::String characterTypeID;
		juce::String oversampleID;

		int index;
		bool oversample = false;
		CharacterType characterType = CharacterType::AsymPositive;

		float distort(float sample, float drive, float character);

	public:
		static const std::vector<DistortionDefinition> distortionDefs;
		static const std::array<juce::String, 7> characterDefs;

		DistortionProcessor(juce::AudioProcessorValueTreeState& params, ModMatrix& modMatrix, const juce::String& driveID, const juce::String& characterID, const juce::String& typeID, const juce::String& characterTypeID, const juce::String& oversampleID);

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
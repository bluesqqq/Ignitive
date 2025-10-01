#pragma once

#define _USE_MATH_DEFINES

#include <vector>
#include <JuceHeader.h>
#include <cmath>
#include "ModMatrix.h"

enum class CharacterType {
	Bend,
	Asym,
	Fold,
	Rectify,
	Quantize
};

enum class DistortionCategory {
	Classic,
	Digital,
	Broken,
	Formant,
	Experimental,
	NumCategories
};

enum class CharacterPolarity {
	Plus,
	Minus,
	PlusMinus
};

static const std::array<juce::String, static_cast<int>(DistortionCategory::NumCategories)> categoryNames = {
	"Classic",
	"Digital",
	"Broken",
	"Formant",
	"Experimental"
};


struct DistortionAlgorithm {
	juce::String name;
	juce::String description;
	DistortionCategory category;
	float (*process)(float);
};

struct CharacterAlgorithm {
	juce::String name;
	juce::String description;
	bool polarized;
	void (*process)(float&, float&, float&);
};

class DistortionProcessor : public juce::dsp::ProcessorBase {
	private:
		static const std::vector<CharacterAlgorithm> characterAlgos;
		static const std::vector<DistortionAlgorithm> distortionAlgos;

		juce::AudioProcessorValueTreeState& parameters;
		ModMatrix& modMatrix;

		std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;

		// Parameter IDs
		juce::String driveID;
		juce::String characterID;
		juce::String typeID;
		juce::String characterTypeID;
		juce::String characterPolarityID;
		juce::String oversampleID;

		bool oversample = false;

		int distortionIndex = 0;
		const DistortionAlgorithm* currentAlgorithm;

		int characterIndex = 0;
		const CharacterAlgorithm* currentCharacterAlgo;
		CharacterPolarity characterPolarity = CharacterPolarity::Plus;

		float distort(float sample, float drive, float character);

	public:
		static const void makeDistortionAlgosMenu(juce::PopupMenu& menu);
		static const juce::StringArray getAlgorithmNames() {
			juce::StringArray result;

			for (const auto& algo : distortionAlgos)
				result.add(algo.name);

			return result;
		}
		static const juce::StringArray getCharacterNames() {
			juce::StringArray result;

			for (const auto& algo : characterAlgos)
				result.add(algo.name);

			return result;
		}

		static const int getNumAlgorithms() { return distortionAlgos.size(); }

		static const bool CharacterHasPolarity(int index) {
			if (index < 0 || index >= characterAlgos.size()) return false;
			return characterAlgos[index].polarized;
		}

		DistortionProcessor(juce::AudioProcessorValueTreeState& params, ModMatrix& modMatrix, const juce::String& driveID, const juce::String& characterID, const juce::String& typeID, const juce::String& characterTypeID, const juce::String& characterPolarityID, const juce::String& oversampleID);

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

		const juce::String& getAlgorithmName(int index = -1) {
			return getDistortionAlgorithm(index)->name;
		}

		const juce::String& getAlgorithmDescription(int index = -1) {
			return getDistortionAlgorithm(index)->description;
		}

		const DistortionAlgorithm* getDistortionAlgorithm(int index = -1) {
			if (index < 0 || index >= distortionAlgos.size()) index = distortionIndex;

			return &distortionAlgos[index];
		}

		const CharacterAlgorithm* getCharacterAlgorithm(int index = -1) {
			if (index < 0 || index >= characterAlgos.size()) index = characterIndex;
			return &characterAlgos[index];
		}
};
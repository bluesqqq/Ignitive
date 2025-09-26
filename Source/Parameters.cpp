#include "Parameters.h"

#include "DistortionProcessor.h"

const std::vector<juce::String> Parameters::randomizeParameters = {
	ID_DRIVE,
	ID_CHARACTER,
	ID_FEEDBACK,
	ID_FEEDBACK_DELAY,
	ID_LP_CUTOFF,
	ID_LP_RESONANCE,
	ID_HP_CUTOFF,
	ID_HP_RESONANCE,
	ID_ENV_ATTACK,
	ID_ENV_DECAY,
	ID_ENV_GATE,
	ID_LFO_SPEED
};

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout() {
	std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_IN_GAIN, "In Gain", 0.0f, 2.0f, 1.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_OUT_GAIN, "Out Gain", 0.0f, 2.0f, 1.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_MIX, "Mix", 0.0f, 1.0f, 1.0f));
	params.push_back(std::make_unique<juce::AudioParameterBool>(ID_BYPASS, "Bypass", false));
	params.push_back(std::make_unique<juce::AudioParameterBool>(ID_LIMITER, "Limiter", true));
	params.push_back(std::make_unique<juce::AudioParameterBool>(ID_SOFTCLIP, "Soft Clip", false));

	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_DRIVE, "Drive", 0.0f, 1.0f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_CHARACTER, "Character", 0.0f, 1.0f, 0.0f));

	juce::StringArray distortionNames;
	for (const auto& def : DistortionProcessor::distortionDefs) distortionNames.add(def.name);
	params.push_back(std::make_unique<juce::AudioParameterChoice>(ID_DISTORTION_TYPE, "Distortion Type", distortionNames, 0));

	juce::StringArray characterNames;
	for (const auto& def : DistortionProcessor::characterDefs) characterNames.add(def);
	params.push_back(std::make_unique<juce::AudioParameterChoice>(ID_CHARACTER_TYPE, "Character Type", characterNames, 0));

	params.push_back(std::make_unique<juce::AudioParameterBool>(ID_OVERSAMPLE, "Oversample", false));

	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_FEEDBACK, "Feedback", 0.0f, 1.0f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_FEEDBACK_DELAY, "Feedback Delay", 0.0f, 1.0f, 0.5f));

	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_LP_CUTOFF,    "LP Cutoff", 0.0f, 1.0f, 1.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_LP_RESONANCE, "LP Resonance", 0.0f, 1.0f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_HP_CUTOFF,    "HP Cutoff", 0.0f, 1.0f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_HP_RESONANCE, "HP Resonance", 0.0f, 1.0f, 0.0f));

	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_ENV_ATTACK, "Envelope Attack", 0.0f, 0.5f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_ENV_DECAY, "Envelope Decay", 0.0f, 1.0f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_ENV_GATE, "Envelope Gate", 0.0f, 1.0f, 0.0f));

	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_LFO_SPEED, "LFO Speed", 0.0f, 1.0f, 0.5f));

	return { params.begin(), params.end() };
}
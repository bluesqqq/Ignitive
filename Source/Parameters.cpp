#include "Parameters.h"

#include "DistortionProcessor.h"

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout() {
	std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_IN_GAIN, "In Gain", 0.0f, 2.0f, 1.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_OUT_GAIN, "Out Gain", 0.0f, 2.0f, 1.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_MIX, "Mix", 0.0f, 1.0f, 1.0f));

	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_DRIVE, "Drive", 0.0f, 1.0f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_COLOR, "Color", 0.0f, 1.0f, 0.0f));

	juce::StringArray distortionNames;
	for (const auto& def : DistortionProcessor::distortionDefs) distortionNames.add(def.name);
	params.push_back(std::make_unique<juce::AudioParameterChoice>(ID_DISTORTION_TYPE, "Distortion Type", distortionNames, 0));

	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_FEEDBACK, "Feedback", 0.0f, 1.0f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_FEEDBACK_DELAY, "Feedback Delay", 0.0f, 1.0f, 0.5f));

	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_PRE_FILTER_CUTOFF, "Pre Filter Cutoff", 0.0f, 1.0f, 0.5f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_PRE_FILTER_RESONANCE, "Pre Filter Resonance", 0.0f, 1.0f, 0.5f));
	params.push_back(std::make_unique<juce::AudioParameterChoice>(ID_PRE_FILTER_TYPE, "Pre Filter Type", juce::StringArray{ "LP", "BP", "HP" }, 0));
	params.push_back(std::make_unique<juce::AudioParameterBool>(ID_PRE_FILTER_ENABLED, "Pre Filter Enabled", false));

	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_POST_FILTER_CUTOFF, "Post Filter Cutoff", 0.0f, 1.0f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_POST_FILTER_RESONANCE, "Post Filter Resonance", 0.0f, 1.0f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterChoice>(ID_POST_FILTER_TYPE, "Post Filter Type", juce::StringArray{ "LP", "BP", "HP" }, 0));
	params.push_back(std::make_unique<juce::AudioParameterBool>(ID_POST_FILTER_ENABLED, "Post Filter Enabled", false));

	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_ENV_ATTACK, "Envelope Attack", 0.0f, 0.5f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_ENV_DECAY, "Envelope Decay", 0.0f, 1.0f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(ID_ENV_GATE, "Envelope Gate", 0.0f, 1.0f, 0.0f));

	return { params.begin(), params.end() };
}
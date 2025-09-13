#pragma once

#include <JuceHeader.h>

class Parameters {
	public:
		inline static const juce::String
			ID_IN_GAIN               = "inGain",
			ID_OUT_GAIN              = "outGain",
			ID_MIX                   = "mix",
			ID_DRIVE                 = "driveAmount",
			ID_COLOR                 = "driveColor",
			ID_DISTORTION_TYPE       = "distortionType",
			ID_FEEDBACK              = "feedbackAmount",
			ID_FEEDBACK_DELAY        = "feedbackDelay",
			ID_PRE_FILTER_CUTOFF     = "preFilterCutoff",
			ID_PRE_FILTER_RESONANCE  = "preFilterResonance",
			ID_PRE_FILTER_TYPE       = "preFilterType",
			ID_PRE_FILTER_ENABLED    = "preFilterEnabled",
			ID_POST_FILTER_CUTOFF    = "postFilterCutoff",
			ID_POST_FILTER_RESONANCE = "postFilterResonance",
			ID_POST_FILTER_TYPE      = "postFilterType",
			ID_POST_FILTER_ENABLED   = "postFilterEnabled",
			ID_ENV_ATTACK            = "envAttack",
			ID_ENV_DECAY             = "envDecay",
			ID_ENV_GATE              = "envGate",

			ID_ENV = "envelope",
			ID_LFO = "lfo";

		inline static const std::vector<juce::String> destinationIDs{
			ID_DRIVE,
			ID_COLOR,
			ID_FEEDBACK,
			ID_FEEDBACK_DELAY,
			ID_PRE_FILTER_CUTOFF,
			ID_PRE_FILTER_RESONANCE,
			ID_POST_FILTER_CUTOFF,
			ID_POST_FILTER_RESONANCE
		};

		inline static const std::vector<juce::String> sourceIDs{
			ID_ENV,
			ID_LFO
		};

		static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

};
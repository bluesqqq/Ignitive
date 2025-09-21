#pragma once

#include <JuceHeader.h>

class Parameters {
	public:
		inline static const juce::String
			ID_IN_GAIN               = "inGain",
			ID_OUT_GAIN              = "outGain",
			ID_MIX                   = "mix",
			ID_BYPASS                = "bypass",
			ID_LIMITER               = "limiter",

			ID_DRIVE                 = "driveAmount",
			ID_CHARACTER             = "driveColor",
			ID_DISTORTION_TYPE       = "distortionType",
			ID_CHARACTER_TYPE        = "characterType",
			ID_OVERSAMPLE            = "oversample",

			ID_FEEDBACK              = "feedbackAmount",
			ID_FEEDBACK_DELAY        = "feedbackDelay",

			ID_LP_CUTOFF    = "filter1_lpCutoff",
			ID_LP_RESONANCE = "filter1_lpResonance",
			ID_HP_CUTOFF    = "filter1_hpCutoff",
			ID_HP_RESONANCE = "filter1_hpResonance",

			ID_ENV_ATTACK            = "envAttack",
			ID_ENV_DECAY             = "envDecay",
			ID_ENV_GATE              = "envGate",

			ID_LFO_SPEED = "lfoSpeed",

			ID_ENV = "envelope",
			ID_LFO = "lfo";

		static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

};
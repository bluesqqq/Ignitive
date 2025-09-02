#pragma once

#include <JuceHeader.h>
#include "SwitchButton.h"

class SwitchButtonAttachment : private juce::AudioProcessorValueTreeState::Listener {
	private:
		void parameterChanged(const juce::String& parameterID, float newValue) override;

		juce::AudioProcessorValueTreeState& apvts;
		juce::String parameterID;
		SwitchButton& button;

	public:
		SwitchButtonAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID, SwitchButton& button);

		~SwitchButtonAttachment() override;
};
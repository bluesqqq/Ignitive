#pragma once

#include <JuceHeader.h>
#include "SwitchButtonAttachment.h"
#include "CustomLAFs.h"
#include "FilterProcessor.h"

class FilterComponent : public juce::Component, private juce::Timer {
    private:
		KnobLAF knobLAF;

        juce::Slider lpCutoffKnob, lpResonanceKnob;
        juce::Slider hpCutoffKnob, hpResonanceKnob;

        juce::AudioProcessorValueTreeState::SliderAttachment lpCutoffAttach, lpResonanceAttach;
        juce::AudioProcessorValueTreeState::SliderAttachment hpCutoffAttach, hpResonanceAttach;

        FilterProcessor& filter;

        float graphBottomDB = -48.0f;
        float graphTopDB = 12.0f;

    public:
        FilterComponent(juce::AudioProcessorValueTreeState& parameters, FilterProcessor& filter);

        void resized() override;

        void paint(juce::Graphics& g) override;

        void timerCallback() override;
};
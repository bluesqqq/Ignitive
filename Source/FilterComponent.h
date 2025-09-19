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

        float calculateLowpassResponse(float frequency, float cutoff, float resonance);
        float calculateHighpassResponse(float frequency, float cutoff, float resonance);


    public:
        FilterComponent(juce::AudioProcessorValueTreeState& parameters, FilterProcessor& filter);

        void resized() override;

        void paint(juce::Graphics& g) override;

        void timerCallback() override;
};
#pragma once

#include <JuceHeader.h>
#include "SwitchButtonAttachment.h"
#include "CustomLAFs.h"
#include "FilterProcessor.h"

class FilterComponent : public juce::Component, private juce::Timer {
    private:

        juce::Slider lpCutoffKnob, lpResonanceKnob;
        juce::Slider hpCutoffKnob, hpResonanceKnob;

        juce::AudioProcessorValueTreeState::SliderAttachment lpCutoffAttach, lpResonanceAttach;
        juce::AudioProcessorValueTreeState::SliderAttachment hpCutoffAttach, hpResonanceAttach;

        FilterProcessor& filter;

        IgnitiveLAF& ignitiveLAF;

        float graphBottomDB = -48.0f;
        float graphTopDB = 12.0f;

        juce::Colour highlightColor = juce::Colour::fromRGB(255, 165, 0);
        juce::Colour backgroundColor = juce::Colour::fromRGB(127, 82, 0);

    public:
        FilterComponent(juce::AudioProcessorValueTreeState& parameters, FilterProcessor& filter, IgnitiveLAF& ignitiveLAF);

        void resized() override;

        void paint(juce::Graphics& g) override;

        void timerCallback() override;
};
#pragma once

#include <JuceHeader.h>
#include "SwitchButtonAttachment.h"
#include "IgnitiveLAF.h"
#include "FilterProcessor.h"

class FilterCurve : public juce::Component, private juce::Timer {
    private:
        FilterProcessor& filter;

        IgnitiveLAF& ignitiveLAF;

        float graphBottomDB = -48.0f;
        float graphTopDB = 12.0f;

        juce::Colour highlightColor = juce::Colour::fromRGB(255, 165, 0);
        juce::Colour backgroundColor = juce::Colour::fromRGB(127, 82, 0);

    public:
        FilterCurve(juce::AudioProcessorValueTreeState& parameters, FilterProcessor& filter, IgnitiveLAF& ignitiveLAF);

        void paint(juce::Graphics& g) override;

        void timerCallback() override;
};
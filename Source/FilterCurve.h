#pragma once

#include <JuceHeader.h>
#include "SwitchButtonAttachment.h"
#include "IgnitiveLAF.h"
#include "FilterProcessor.h"
#include "Globals.h"

class FilterCurve : public juce::Component {
    private:
        FilterProcessor& filter;

        IgnitiveLAF& ignitiveLAF;

        float graphBottomDB = -48.0f;
        float graphTopDB = 12.0f;

        juce::Colour highlightColor = Globals::filterColor;
        juce::Colour backgroundColor = Globals::filterColor.darker(Globals::dimAmount);

    public:
        FilterCurve(juce::AudioProcessorValueTreeState& parameters, FilterProcessor& filter, IgnitiveLAF& ignitiveLAF);

        void paint(juce::Graphics& g) override;
};
#pragma once

#include <JuceHeader.h>
#include "GainProcessor.h"

class LevelMeter : public juce::Component, private juce::Timer {
    private:
        GainProcessor& gainProcessor;

    public:
        LevelMeter(GainProcessor& gainProcessor);

        void paint(juce::Graphics& g) override;

        void timerCallback() override;
};
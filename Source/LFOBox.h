#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class LFOBox : public juce::Component, private juce::Timer {
private:
    std::deque<float> lfoValues;
    unsigned int segments = 100;
    IgnitiveAudioProcessor& audioProcessor;

public:
    LFOBox(IgnitiveAudioProcessor& ap) : audioProcessor(ap) {
        startTimer(1000 / 60);
    }

    void paint(juce::Graphics& g) override;

    void pushLFOValue(float newValue);

    void timerCallback() override;
};

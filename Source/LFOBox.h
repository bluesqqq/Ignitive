#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class LFOBox : public juce::Component, private juce::Timer {
private:
    std::deque<float> lfoValues;
    IgnitiveAudioProcessor& audioProcessor;

    static constexpr int pixelSize = 5;

    juce::Colour highlightColor = juce::Colour::fromRGB(255, 165, 0);
    juce::Colour backgroundColor = juce::Colour::fromRGB(127, 82, 0);

public:
    LFOBox(IgnitiveAudioProcessor& ap) : audioProcessor(ap) {
        startTimerHz(60);
    }

    void paint(juce::Graphics& g) override;

    void pushLFOValue(float newValue);

    void timerCallback() override;
};

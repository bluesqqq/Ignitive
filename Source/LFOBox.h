#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class LFOBox : public juce::Component {
private:
    std::deque<float> lfoValues;
    IgnitiveAudioProcessor& audioProcessor;

    static constexpr int pixelSize = 5;

    juce::Colour highlightColor = juce::Colour::fromRGB(255, 165, 0);
    juce::Colour backgroundColor = juce::Colour::fromRGB(127, 82, 0);

public:
    LFOBox(IgnitiveAudioProcessor& ap) : audioProcessor(ap) {}

    void paint(juce::Graphics& g) override;

    void pushLFOValue(float newValue);
};

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class EnvelopeBox : public juce::Component, private juce::Timer {
    private:
        std::deque<float> envelopeValues;
        IgnitiveAudioProcessor& audioProcessor;

        juce::Colour highlightColor  = juce::Colour::fromRGB(255, 255, 0);
        juce::Colour backgroundColor = juce::Colour::fromRGB(127, 127, 0);

    public:
        EnvelopeBox(IgnitiveAudioProcessor& ap) : audioProcessor(ap) {
            startTimer(1000 / 60);
        }

        void paint(juce::Graphics& g) override;

        void pushEnvelopeValue(float newValue);

        void timerCallback() override;
};

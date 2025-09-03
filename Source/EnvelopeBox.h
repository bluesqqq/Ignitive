#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class EnvelopeBox : public juce::Component, private juce::MultiTimer {
    private:
        std::deque<float> envelopeValues;
		unsigned int segments = 100;
        IgnitiveAudioProcessor& audioProcessor;

    public:
        EnvelopeBox(IgnitiveAudioProcessor& ap) : audioProcessor(ap) {
            startTimer(0, 1000 / 60);
            startTimer(1, 2);
        }

        void paint(juce::Graphics& g) override;

        void pushEnvelopeValue(float newValue);

        void timerCallback(int timerID) override;

};

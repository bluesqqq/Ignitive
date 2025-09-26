#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class EnvelopeBox : public juce::Component {
    private:
        std::deque<float> envelopeValues;
        IgnitiveAudioProcessor& audioProcessor;

        juce::Colour highlightColor  = juce::Colour::fromRGB(255, 255, 0);
        juce::Colour backgroundColor = juce::Colour::fromRGB(127, 127, 0);

    public:
        EnvelopeBox(IgnitiveAudioProcessor& ap) : audioProcessor(ap) {}

        void paint(juce::Graphics& g) override;

        void pushEnvelopeValue(float newValue);
};

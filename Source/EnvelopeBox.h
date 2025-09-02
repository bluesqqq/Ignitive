#pragma once

#include <JuceHeader.h>

class EnvelopeBox : public juce::Component {
    private:
        std::deque<float> envelopeValues;
		unsigned int segments = 100;

    public:
        EnvelopeBox() { }

        void paint(juce::Graphics& g) override;

        void pushEnvelopeValue(float newValue);
};

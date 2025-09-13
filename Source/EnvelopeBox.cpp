#include "EnvelopeBox.h"

void EnvelopeBox::paint(juce::Graphics& g) {
    auto envBox = getLocalBounds().toFloat().reduced(10);
    g.setColour(juce::Colours::yellow);
    g.drawRect(envBox, 2.0f);

    float segmentWidth = envBox.getWidth() / (float)segments;

    juce::Path path;

    for (int i = 0; i < envelopeValues.size(); ++i) {
        float x = i * segmentWidth;
        float y = envBox.getHeight() * 0.9 - envelopeValues[i] * (envBox.getHeight() * 0.8);

        if (i == 0) path.startNewSubPath(x, y);
        else        path.lineTo(x, y);
    }

    g.setColour(juce::Colours::yellow);
    g.strokePath(path, juce::PathStrokeType(2.0f));
}

void EnvelopeBox::pushEnvelopeValue(float newValue) {
    envelopeValues.push_back(newValue);
    if (envelopeValues.size() > segments)
        envelopeValues.pop_front();
}

void EnvelopeBox::timerCallback() {
    auto& follower = audioProcessor.ignitive.envelope;

    while (follower.popFifo(500)) {
        float v = follower.readFifo();
        pushEnvelopeValue(v);
    }

    repaint();
}

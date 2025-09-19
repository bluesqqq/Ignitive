#include "LFOBox.h"

void LFOBox::paint(juce::Graphics& g) {
    auto lfoBox = getLocalBounds().toFloat().reduced(10);
    g.setColour(juce::Colours::orange);
    g.drawRect(lfoBox, 3.0f);

    float segmentWidth = lfoBox.getWidth() / (float)segments;

    juce::Path path;

    for (int i = 0; i < lfoValues.size(); ++i) {
        float val = lfoValues[i] / 2.0f + 0.5f;
        float x = i * segmentWidth;
        float y = lfoBox.getHeight() * 0.9 - val * (lfoBox.getHeight() * 0.8);

        if (i == 0) path.startNewSubPath(x, y);
        else        path.lineTo(x, y);
    }

    g.setColour(juce::Colours::orange);
    g.strokePath(path, juce::PathStrokeType(3.0f));
}

void LFOBox::pushLFOValue(float newValue) {
    lfoValues.push_back(newValue);
    if (lfoValues.size() > segments)
        lfoValues.pop_front();
}


void LFOBox::timerCallback() {
    auto& follower = audioProcessor.ignitive.lfo;

    while (follower.popFifo(500)) {
        float v = follower.readFifo();
        pushLFOValue(v);
    }

    repaint();
}
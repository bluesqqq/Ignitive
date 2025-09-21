#include "LFOBox.h"

void LFOBox::paint(juce::Graphics& g) {
    auto lfoBox = getLocalBounds().toFloat().reduced(5.0f);
    g.setColour(juce::Colours::orange);

    const int pixelSize = 5;
    int wPixels = (int)lfoBox.getWidth() / pixelSize;
    int hPixels = (int)lfoBox.getHeight() / pixelSize;

    while (lfoValues.size() > wPixels)
        lfoValues.pop_front();

    while (lfoValues.size() < wPixels)
        lfoValues.push_front(0.0f);

    float midY = lfoBox.getY() + lfoBox.getHeight() / 2.0f;

    for (int ix = 0; ix < lfoValues.size(); ++ix) {
        float val = juce::jlimit(-1.0f, 1.0f, lfoValues[ix]);

        int pixelCount = (int)(std::abs(val) * hPixels / 2.0f);

        for (int iy = 0; iy < pixelCount; ++iy) {
            float y;
            if (val >= 0) {
                y = midY - (iy + 1) * pixelSize;
            } else {
                y = midY + iy * pixelSize;
            }

            juce::Rectangle<float> pixel(lfoBox.getX() + ix * pixelSize, y, pixelSize - 1.0f, pixelSize - 1.0f);

            if (iy == pixelCount - 1)
                g.setColour(highlightColor);
            else
                g.setColour(backgroundColor);

            g.fillRect(pixel);
        }
    }
}

void LFOBox::pushLFOValue(float newValue) {
    lfoValues.push_back(newValue);
}


void LFOBox::timerCallback() {
    auto& follower = audioProcessor.ignitive.lfo;

    while (follower.popFifo(900)) {
        float v = follower.readFifo();
        pushLFOValue(v);
    }

    repaint();
}
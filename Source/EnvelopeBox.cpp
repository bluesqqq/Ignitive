#include "EnvelopeBox.h"
#include "LFOBox.h"

void EnvelopeBox::paint(juce::Graphics& g) {
    auto& follower = audioProcessor.ignitive.envelope;

    while (follower.popFifo(900)) {
        float v = follower.readFifo();
        pushEnvelopeValue(v);
    }

    auto envBox = getLocalBounds().toFloat().reduced(5.0f);
    g.setColour(juce::Colours::yellow);

    const int pixelSize = 5; // horizontal and vertical pixel size
    int wPixels = (int)envBox.getWidth() / pixelSize;
    int hPixels = (int)envBox.getHeight() / pixelSize;

    // Trim or pad the envelope deque to match width
    while (envelopeValues.size() > wPixels)
        envelopeValues.pop_front();
    while (envelopeValues.size() < wPixels)
        envelopeValues.push_front(0.0f);

    float bottomY = envBox.getY() + envBox.getHeight();

    for (int ix = 0; ix < envelopeValues.size(); ++ix) {
        float val = juce::jlimit(0.0f, 1.0f, envelopeValues[ix]); // 0..1
        int pixelCount = (int)(val * hPixels); // number of pixels to fill

        for (int iy = 0; iy < pixelCount; ++iy) {
            float y = bottomY - (iy + 1) * pixelSize; // draw upward from bottom
            juce::Rectangle<float> pixel(envBox.getX() + ix * pixelSize, y, pixelSize - 1.0f, pixelSize - 1.0f);

            if (iy == pixelCount - 1)
                g.setColour(highlightColor);
            else
                g.setColour(backgroundColor);

            g.fillRect(pixel);
        }
    }
}


void EnvelopeBox::pushEnvelopeValue(float newValue) {
    envelopeValues.push_back(newValue);
}

#include "ModSourceGraph.h"

ModSourceGraph::ModSourceGraph(IgnitiveAudioProcessor& ap, ModSource* src) : audioProcessor(ap), source(src) {}

void ModSourceGraph::setSource(ModSource* src) {
    source = src;
    values.clear();
}

void ModSourceGraph::paint(juce::Graphics& g) {
    if (source == nullptr)
        return;

    while (source->popFifo(500))
        values.push_back(source->readFifo());

    auto box = getLocalBounds().toFloat().reduced(5.0f);

    int wPixels = (int)box.getWidth() / pixelSize;
    int hPixels = (int)box.getHeight() / pixelSize;

    while (values.size() > wPixels) values.pop_front();
    while (values.size() < wPixels) values.push_front(0.0f);

    const auto range = source->displayRange;
    const juce::Colour color = source->displayColor;

    auto normalise = [&](float value) { return juce::jmap(value, range.getStart(), range.getEnd(), 0.0f, 1.0f); };

    float pivotNorm = normalise(0.0f);

    for (int ix = 0; ix < values.size(); ++ix) {
        float valueNorm = normalise(values[ix]);
        float pivotPixelFloat = pivotNorm * (hPixels - 1);
        float valuePixelFloat = valueNorm * hPixels;

        /*
            This is how the pixels should be displayed for 0 value pivots

            1.0f - Odd number of pixels
            [] -
            [] - [0] - centered perfectly on pixel
            [] -
            -1.0f

            When values > 0, the pivot is rounded up, so the first pixel drawn is the one above the pivot
            When values < 0, the pivot is rounded down, so the first pixel drawn is the one below the pivot

            1.0f - Even number of pixels
            [] -
            [] - Value > 0
                 [0] - centered between pixels above and below
            [] - Value < 0
            [] -
            -1.0f

            This doesnt account if the range was reversed (-1.0f at the top, 1.0f at the bottom)
        */

        int pixelsStart;
        if (values[ix] >= 0.0f)
            pixelsStart = (int)std::ceil(pivotPixelFloat);
        else
            pixelsStart = (int)std::floor(pivotPixelFloat);

        int pixelsEnd = (int)valuePixelFloat;

        int top = juce::jmin(pixelsStart, pixelsEnd);
        top = juce::jlimit(0, hPixels - 1, top);
        int bottom = juce::jmax(pixelsStart, pixelsEnd);
        bottom = juce::jlimit(0, hPixels - 1, bottom);

        for (int iy = top; iy <= bottom; ++iy) {
            juce::Rectangle<float> pixel(box.getX() + ix * pixelSize, box.getY() + box.getHeight() - iy * pixelSize - pixelSize, pixelSize - 1.0f, pixelSize - 1.0f);

            bool isEndPixel = (iy == pixelsEnd);
            g.setColour(isEndPixel ? color : color.darker(0.7f));
            g.fillRect(pixel);
        }

    }
}
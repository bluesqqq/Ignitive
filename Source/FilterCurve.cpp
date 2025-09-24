#include "FilterCurve.h"

FilterCurve::FilterCurve(juce::AudioProcessorValueTreeState& params, FilterProcessor& filter, IgnitiveLAF& ignitiveLAF)
    : filter(filter), ignitiveLAF(ignitiveLAF) {
    startTimer(1000 / 60);
}

void FilterCurve::paint(juce::Graphics& g) {
    auto screen = getLocalBounds().toFloat();
    screen.reduce(5.0f, 5.0f);

    const int pixelSize = 5;
    int wPixels = (int)screen.getWidth() / pixelSize;
    int hPixels = (int)screen.getHeight() / pixelSize;

    float lpCutoff = filter.lpFilter.getCutoffFrequency();
    float lpResonance = filter.lpFilter.getResonance();

    float hpCutoff = filter.hpFilter.getCutoffFrequency();
    float hpResonance = filter.hpFilter.getResonance();

    // Prepare horizontal frequency mapping
    std::vector<float> filterCurve;
    filterCurve.reserve(wPixels);

    auto lpCoeffs = *juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, lpCutoff, lpResonance);
    auto hpCoeffs = *juce::dsp::IIR::Coefficients<float>::makeHighPass(44100, hpCutoff, hpResonance);

    for (int ix = 0; ix < wPixels; ++ix) {
        float pos = (float)ix / (float)(wPixels - 1);
        float freq = 20.0f * std::pow(10.0f, pos * 3.0f); // log scale 20Hz-20kHz
        float omega = 2.0f * juce::MathConstants<float>::pi * freq / 44100;
        std::complex<float> z = std::polar(1.0f, -omega);

        // Evil ahh formula
        std::complex<float> Hlp = (lpCoeffs.coefficients[0] + lpCoeffs.coefficients[1] * z + lpCoeffs.coefficients[2] * z * z)
            / (1.0f + lpCoeffs.coefficients[3] * z + lpCoeffs.coefficients[4] * z * z);

        std::complex<float> Hhp = (hpCoeffs.coefficients[0] + hpCoeffs.coefficients[1] * z + hpCoeffs.coefficients[2] * z * z)
            / (1.0f + hpCoeffs.coefficients[3] * z + hpCoeffs.coefficients[4] * z * z);

        float combined = std::abs(Hlp * Hhp);
        float db = 20.0f * std::log10(std::max(combined, 1e-6f));
        float normalized = juce::jlimit(0.0f, 1.0f, juce::jmap(db, graphBottomDB, graphTopDB, 0.0f, 1.0f));

        filterCurve.push_back(normalized);
    }

    // Draw pixels
    g.setColour(juce::Colours::green);
    float bottomY = screen.getY() + screen.getHeight();
    float cx = screen.getY() + screen.getWidth() / 2;
    float cy = 200.0f;
    float radius = 280.0f / 2.0f;

    for (int ix = 0; ix < filterCurve.size(); ++ix) {
        int pixelCount = (int)(filterCurve[ix] * hPixels);

        for (int iy = 0; iy < pixelCount; ++iy) {
            float y = bottomY - (iy + 1) * pixelSize;
            float px = screen.getX() + ix * pixelSize + pixelSize / 2.0f;
            float py = y + pixelSize / 2.0f;

            // only draw if outside the circle
            if ((px - cx) * (px - cx) + (py - cy) * (py - cy) > radius * radius) {
                juce::Rectangle<float> pixel(screen.getX() + ix * pixelSize, y, pixelSize - 1.0f, pixelSize - 1.0f);

                if (iy == pixelCount - 1) // top pixel in this column
                    g.setColour(highlightColor);
                else
                    g.setColour(backgroundColor);

                g.fillRect(pixel);
            }
        }
    }
}



void FilterCurve::timerCallback() {
    repaint();
}

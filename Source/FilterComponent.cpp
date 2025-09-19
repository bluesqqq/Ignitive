#include "FilterComponent.h"

FilterComponent::FilterComponent(juce::AudioProcessorValueTreeState& params, FilterProcessor& filter)
    : lpCutoffAttach(params, filter.lpCutoffID, lpCutoffKnob),
    lpResonanceAttach(params, filter.lpResonanceID, lpResonanceKnob),
    hpCutoffAttach(params, filter.hpCutoffID, hpCutoffKnob),
    hpResonanceAttach(params, filter.hpResonanceID, hpResonanceKnob),
    filter(filter) {
    startTimer(1000 / 60);

    lpCutoffKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    lpCutoffKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lpCutoffKnob.setLookAndFeel(&knobLAF);
    lpCutoffKnob.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(lpCutoffKnob);

    lpResonanceKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    lpResonanceKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lpResonanceKnob.setLookAndFeel(&knobLAF);
    lpResonanceKnob.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(lpResonanceKnob);

    hpCutoffKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    hpCutoffKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    hpCutoffKnob.setLookAndFeel(&knobLAF);
    hpCutoffKnob.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(hpCutoffKnob);

    hpResonanceKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    hpResonanceKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    hpResonanceKnob.setLookAndFeel(&knobLAF);
    hpResonanceKnob.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(hpResonanceKnob);
}

void FilterComponent::resized() {
    // Highpass on left, lowpass on right
    hpCutoffKnob.setBounds(40, 130, 60, 60);
    hpResonanceKnob.setBounds(40, 220, 40, 40);
    lpCutoffKnob.setBounds(380, 130, 60, 60);
    lpResonanceKnob.setBounds(400, 220, 40, 40);
}

void FilterComponent::paint(juce::Graphics& g) {
    juce::Rectangle<float> screen(110.0f, 120.0f, 260.0f, 80.0f);
    screen.reduce(5.0f, 5.0f);
    juce::Path path;

    float lpCutoff = filter.lpFilter.getCutoffFrequency();
    float lpResonance = filter.lpFilter.getResonance();

    float hpCutoff = filter.hpFilter.getCutoffFrequency();
    float hpResonance = filter.hpFilter.getResonance();

    int numberOfPoints = 200; // more points = smoother curve
    std::vector<float> filterCurve;
    filterCurve.reserve(numberOfPoints);

    auto lpCoeffs = *juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, lpCutoff, lpResonance);
    auto hpCoeffs = *juce::dsp::IIR::Coefficients<float>::makeHighPass(44100, hpCutoff, hpResonance);

    for (int i = 0; i < numberOfPoints; ++i) {
        float pos = (float)i / (float)(numberOfPoints - 1);
        float freq = 20.0f * std::pow(10.0f, pos * 3.0f); // 20Hz-20kHz log scale
        float omega = 2.0f * juce::MathConstants<float>::pi * freq / 44100;
        std::complex<float> z = std::polar(1.0f, -omega);

        std::complex<float> Hlp = (lpCoeffs.coefficients[0] + lpCoeffs.coefficients[1] * z + lpCoeffs.coefficients[2] * z * z)
            / (1.0f + lpCoeffs.coefficients[3] * z + lpCoeffs.coefficients[4] * z * z);

        std::complex<float> Hhp = (hpCoeffs.coefficients[0] + hpCoeffs.coefficients[1] * z + hpCoeffs.coefficients[2] * z * z)
            / (1.0f + hpCoeffs.coefficients[3] * z + hpCoeffs.coefficients[4] * z * z);

        float combined = std::abs(Hlp * Hhp);

        float db = 20.0f * std::log10(std::max(combined, 1e-6f));
        float normalized = juce::jlimit(0.0f, 1.0f, juce::jmap(db, graphBottomDB, graphTopDB, 0.0f, 1.0f));

        filterCurve.push_back(normalized);
    }

    float width = screen.getWidth() / (float)filterCurve.size();
    for (int i = 0; i < filterCurve.size(); ++i) {
        float x = screen.getX() + width * i;
        float y = juce::jmap(filterCurve[i], screen.getY() + screen.getHeight(), screen.getY());
        if (i == 0) path.startNewSubPath(x, y);
        else path.lineTo(x, y);
    }

    g.setColour(juce::Colours::green);
    g.strokePath(path, juce::PathStrokeType(3.0f));
}


void FilterComponent::timerCallback() {
    repaint();
}

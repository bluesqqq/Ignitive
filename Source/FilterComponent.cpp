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

    float lpCutoff    = filter.lpFilter.getCutoffFrequency();
    float lpResonance = filter.lpFilter.getResonance();

    float hpCutoff = filter.hpFilter.getCutoffFrequency();
    float hpResonance = filter.hpFilter.getResonance();

    int numberOfPoints = 100;
    std::vector<float> filterCurve;
    filterCurve.reserve(numberOfPoints);

    for (int i = 0; i < numberOfPoints; i++) {
        float pos = (float)i / (float)(numberOfPoints - 1);
        float hz = 20.0f * std::pow(10.0f, pos * 3.0f);

        float hpResponse = calculateHighpassResponse(hz, hpCutoff, hpResonance); 
        float lpResponse = calculateLowpassResponse(hz, lpCutoff, lpResonance);

        float combinedResponse = hpResponse * lpResponse;

        // Convert to dB and normalize for display
        float responseDB = 20.0f * std::log10(std::max(combinedResponse, 0.00001f));

        // Map dB range to 0-1 for display (-48dB to +6dB range)
        float normalizedResponse = juce::jmap(responseDB, -48.0f, 6.0f, 0.0f, 1.0f);
        normalizedResponse = juce::jlimit(0.0f, 1.0f, normalizedResponse);

        filterCurve.push_back(normalizedResponse);
    }

    // Map filter points to line
    float width = screen.getWidth() / (float)filterCurve.size();
    for (int i = 0; i < filterCurve.size(); i++) {
        float x = screen.getX() + width * i;
        float y = juce::jmap(filterCurve[i], screen.getY() + screen.getHeight(), screen.getY());

        if (i == 0) path.startNewSubPath(x, y);
        else path.lineTo(x, y);
    }

    g.setColour(juce::Colours::green);
    g.strokePath(path, juce::PathStrokeType(2.0f));
}

float FilterComponent::calculateHighpassResponse(float frequency, float cutoff, float resonance) {
    if (frequency <= 0.0f || cutoff <= 0.0f) return 0.0f;

    float omega = frequency / cutoff;
    float omega2 = omega * omega;

    float R = 2.0f * (1.0f / std::max(resonance, 0.01f));

    float numerator = omega2 * omega2;
    float denominator = omega2 * omega2 + R * omega2 + 1.0f;

    return std::sqrt(numerator / denominator);
}

float FilterComponent::calculateLowpassResponse(float frequency, float cutoff, float resonance) {
    if (frequency <= 0.0f || cutoff <= 0.0f) return 1.0f;

    float omega = frequency / cutoff;
    float omega2 = omega * omega;
    float R = 2.0f * (1.0f / std::max(resonance, 0.01f));
    float denominator = omega2 * omega2 + R * omega2 + 1.0f;

    return 1.0f / std::sqrt(denominator);
}

void FilterComponent::timerCallback() {
    repaint();
}

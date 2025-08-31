/*
  ==============================================================================

    DriveLAF.cpp
    Created: 31 Aug 2025 12:24:11pm
    Author:  blues

  ==============================================================================
*/

#include "DriveLAF.h"

void DriveLAF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) {
    float angle = juce::jmap(sliderPos, rotaryStartAngle, rotaryEndAngle);
    int centerX = x + width / 2;
    int centerY = y + height / 2;
    float radius = width / 2 - 15;

    juce::Path arc;
    arc.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, angle, true);

    g.setColour(juce::Colours::red);
    g.strokePath(arc, juce::PathStrokeType(5, juce::PathStrokeType::mitered, juce::PathStrokeType::butt));
}
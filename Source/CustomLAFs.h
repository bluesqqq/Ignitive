#pragma once

#include <JuceHeader.h>
#include "DistortionProcessor.h"

class DriveLAF : public juce::LookAndFeel_V4 {
    private:
        DistortionProcessor& distortion;

    public:
        DriveLAF(DistortionProcessor& engine);
        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider);
};

class KnobLAF : public juce::LookAndFeel_V4 {
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider);
};
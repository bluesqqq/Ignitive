#pragma once

#include <JuceHeader.h>
#include "DistortionEngine.h"

class DriveLAF : public juce::LookAndFeel_V4 {
    private:
        DistortionEngine& distortion;

    public:
        DriveLAF(DistortionEngine& engine);
        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider);
};

class KnobLAF : public juce::LookAndFeel_V4 {
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider);
};
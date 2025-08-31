#pragma once

#include <JuceHeader.h>

class DriveLAF : public juce::LookAndFeel_V4 {
    public:
        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider);
};
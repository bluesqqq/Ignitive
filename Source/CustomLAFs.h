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

class ModSlotLAF : public juce::LookAndFeel_V4 {
    public:
        void drawLinearSlider(juce::Graphics& g , int x, int y, int width, int height,
            float sliderPos, float minSliderPos, float maxSliderPos,
            juce::Slider::SliderStyle sliderStyle, juce::Slider& slider);

        void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
            int buttonX, int buttonY, int buttonW, int buttonH,
            juce::ComboBox& comboBox);

        void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};
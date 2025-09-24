#pragma once

#include <JuceHeader.h>
#include "DistortionProcessor.h"

class MixLAF : public juce::LookAndFeel_V4 {
    private:
        juce::Colour highlightColor = juce::Colour::fromRGB(255, 0, 0);
        juce::Colour backgroundColor = juce::Colour::fromRGB(127, 0, 0);

        float mouseFadeRadius = 25.0f;
        float mouseFadeDepth = 0.5f;

    public:
        void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
            float sliderPos, float minSliderPos, float maxSliderPos,
            juce::Slider::SliderStyle sliderStyle, juce::Slider& slider) override;
};

class DriveLAF : public juce::LookAndFeel_V4 {
    private:
        DistortionProcessor& distortion;

    public:
        DriveLAF(DistortionProcessor& engine);
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
};

class BirdsEyeLAF : public juce::LookAndFeel_V4 {
    private:
        float eyelidPosition = 0.0f;
        float targetEyelidPosition = 0.0f;
        bool blinking = false;

        float blinkInterval = 6.0f;
        float lastBlinkTime = 0.0f;
        float lastFrameTime = 0.0f;

        juce::Point<float> pupilLook;
        juce::Point<float> pupilLookTarget;

        int pupilTrackState;
        DistortionProcessor& distortion;

    public:
        BirdsEyeLAF(DistortionProcessor& distortion) : distortion(distortion) {
            pupilLook.setXY(0.0f, 0.0f);
            pupilLookTarget.setXY(0.0f, 0.0f);
        }

        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider);
};
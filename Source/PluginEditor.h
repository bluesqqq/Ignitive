#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "FilterComponent.h"
#include "DriveLAF.h"

class OtherLookAndFeel : public juce::LookAndFeel_V4 {
    private:

    public:
        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) {
            g.setColour(juce::Colours::white);
            g.fillEllipse(x, y, width, height);

            g.setColour(juce::Colours::black);
            g.drawEllipse(x, y, width, height, 3);
            
            float angle = juce::jmap(sliderPos, rotaryStartAngle, rotaryEndAngle);
            float radius = width / 2;
            int centerX = x + width / 2;
            int centerY = y + height / 2;
            g.drawLine(centerX, centerY, centerX + cos(angle) * radius, centerX + sin(angle) * radius, 3);
        }
};

class IgnitiveAudioProcessorEditor  : public juce::AudioProcessorEditor {
    private:
        IgnitiveAudioProcessor& audioProcessor;

        OtherLookAndFeel lookAndFeel;

        DriveLAF driveLAF;

        // Gain
        juce::Slider inGainSlider, mixSlider, outGainSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment inGainAttach{ audioProcessor.apvts, "inGain", inGainSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment mixAttach{ audioProcessor.apvts, "mix", mixSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment outGainAttach{ audioProcessor.apvts, "outGain", outGainSlider };

		// Distortion + Feedback
        juce::Slider driveSlider, colorSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment driveAttach{ audioProcessor.apvts, "drive", driveSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment colorAttach{ audioProcessor.apvts, "color", colorSlider };

        juce::ComboBox distortionTypeSelector;
        juce::AudioProcessorValueTreeState::ComboBoxAttachment distortionTypeAttach{ audioProcessor.apvts, "distortionType", distortionTypeSelector };

        juce::Slider feedbackSlider, feedbackDelaySlider;
        juce::AudioProcessorValueTreeState::SliderAttachment feedbackAttach{ audioProcessor.apvts, "feedback", feedbackSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment feedbackDelayAttach{ audioProcessor.apvts, "feedbackDelay", feedbackDelaySlider };

        // Filters
		FilterComponent preFilterComponent { audioProcessor.apvts, "pre" };
		FilterComponent postFilterComponent { audioProcessor.apvts, "post" };

        juce::TextButton filterToggleButton{ "PRE" };
        bool showingPre = true;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IgnitiveAudioProcessorEditor)

    public:
        IgnitiveAudioProcessorEditor (IgnitiveAudioProcessor&);
        ~IgnitiveAudioProcessorEditor() override;

        void paint (juce::Graphics&) override;
        void resized() override;
};

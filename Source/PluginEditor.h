#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "FilterComponent.h"
#include "CustomLAFs.h"
#include "SwitchButtonAttachment.h"
#include "EnvelopeBox.h"

class IgnitiveAudioProcessorEditor  : public juce::AudioProcessorEditor {
    private:
        IgnitiveAudioProcessor& audioProcessor;

        KnobLAF knobLAF;

        DriveLAF driveLAF;

        juce::Image backgroundImage;

		EnvelopeBox envBox;

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

        SwitchButton filterToggleButton { "Filter Toggle", 2, RIGHT };

        // Envelope + LFO
        juce::Slider attackSlider, decaySlider, gateSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment attackAttach{ audioProcessor.apvts, "envAttack", feedbackSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment decayAttach{ audioProcessor.apvts, "envDecay", feedbackSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment gateAttach{ audioProcessor.apvts, "envGate", feedbackSlider };

        SwitchButton envLFOToggleButton{ "Env LFO Toggle", 2, DOWN };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IgnitiveAudioProcessorEditor)

    public:
        IgnitiveAudioProcessorEditor (IgnitiveAudioProcessor&);
        ~IgnitiveAudioProcessorEditor() override;

        void paint (juce::Graphics&) override;
        void resized() override;
};

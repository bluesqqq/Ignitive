#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class IgnitiveAudioProcessorEditor  : public juce::AudioProcessorEditor {
    private:
        IgnitiveAudioProcessor& audioProcessor;

        juce::Slider inGainSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment inGainAttach{ audioProcessor.apvts, "inGain", inGainSlider };

		juce::Slider outGainSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment outGainAttach{ audioProcessor.apvts, "outGain", outGainSlider };

        juce::Slider feedbackSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment feedbackAttach{ audioProcessor.apvts, "feedback", feedbackSlider };

        juce::Slider feedbackDelaySlider;
        juce::AudioProcessorValueTreeState::SliderAttachment feedbackDelayAttach{ audioProcessor.apvts, "feedbackDelay", feedbackDelaySlider };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IgnitiveAudioProcessorEditor)

    public:
        IgnitiveAudioProcessorEditor (IgnitiveAudioProcessor&);
        ~IgnitiveAudioProcessorEditor() override;

        void paint (juce::Graphics&) override;
        void resized() override;
};

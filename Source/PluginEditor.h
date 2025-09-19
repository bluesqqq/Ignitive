#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "FilterComponent.h"
#include "CustomLAFs.h"
#include "SwitchButtonAttachment.h"
#include "EnvelopeBox.h"
#include "ModMatrixComponent.h"
#include "DriveKnob.h"
#include "Parameters.h"

class IgnitiveAudioProcessorEditor  : public juce::AudioProcessorEditor {
    private:
        IgnitiveAudioProcessor& audioProcessor;

        DistortionLAF distortionLAF;

        KnobLAF knobLAF;

        BirdsEyeLAF birdsEyeLAF;

        juce::Image backgroundImage;

		EnvelopeBox envBox;

        juce::Viewport modMatrixViewport;
        ModMatrixComponent modMatrixComponent;

        // Gain
        juce::Slider inGainSlider, mixSlider, outGainSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment inGainAttach{ audioProcessor.parameters, Parameters::ID_IN_GAIN, inGainSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment mixAttach{ audioProcessor.parameters, Parameters::ID_MIX, mixSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment outGainAttach{ audioProcessor.parameters, Parameters::ID_OUT_GAIN, outGainSlider };

		// Distortion
		DriveKnob driveKnob{ audioProcessor, Parameters::ID_DRIVE };

        juce::ComboBox distortionTypeSelector;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> distortionTypeAttach;

        juce::Slider colorSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment colorAttach{ audioProcessor.parameters, Parameters::ID_CHARACTER, colorSlider };

        juce::ComboBox characterTypeSelector;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> characterTypeAttach;

        // Feedback
        juce::Slider feedbackSlider, feedbackDelaySlider;
        juce::AudioProcessorValueTreeState::SliderAttachment feedbackAttach{ audioProcessor.parameters, Parameters::ID_FEEDBACK, feedbackSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment feedbackDelayAttach{ audioProcessor.parameters, Parameters::ID_FEEDBACK_DELAY, feedbackDelaySlider };

        // Filter
		FilterComponent filterComponent { audioProcessor.parameters, audioProcessor.ignitive.filter };

        // Envelope + LFO
        juce::Slider attackSlider, decaySlider, gateSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment attackAttach{ audioProcessor.parameters, Parameters::ID_ENV_ATTACK, attackSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment decayAttach{ audioProcessor.parameters, Parameters::ID_ENV_DECAY, decaySlider };
        juce::AudioProcessorValueTreeState::SliderAttachment gateAttach{ audioProcessor.parameters, Parameters::ID_ENV_GATE, gateSlider };

        SwitchButton envLFOToggleButton{ "Env LFO Toggle", 2, DOWN };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IgnitiveAudioProcessorEditor)

    public:
        IgnitiveAudioProcessorEditor (IgnitiveAudioProcessor&);
        ~IgnitiveAudioProcessorEditor() override;

        void paint (juce::Graphics&) override;
        void resized() override;
};

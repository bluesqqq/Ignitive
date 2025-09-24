#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "FilterComponent.h"
#include "CustomLAFs.h"
#include "IgnitiveLAF.h"
#include "SwitchButtonAttachment.h"
#include "EnvelopeBox.h"
#include "LFOBox.h"
#include "ModMatrixComponent.h"
#include "DriveKnob.h"
#include "Parameters.h"
#include "LevelMeter.h"

class IgnitiveAudioProcessorEditor  : public juce::AudioProcessorEditor {
    private:
        IgnitiveAudioProcessor& audioProcessor;

        // Fonts
        juce::Font digitalFont;
        juce::Font uavosdFont;

        // Look And Feels
        IgnitiveLAF ignitiveLAF{ uavosdFont, digitalFont };
        MixLAF mixLAF;

        BirdsEyeLAF birdsEyeLAF;

        juce::Image backgroundImage;

		EnvelopeBox envBox;
        LFOBox lfoBox;

        juce::Viewport modMatrixViewport;
        ModMatrixComponent modMatrixComponent;

        LevelMeter inMeter;
        LevelMeter outMeter;

        // Gain
        juce::Slider inGainSlider, mixSlider, outGainSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment inGainAttach{ audioProcessor.parameters, Parameters::ID_IN_GAIN, inGainSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment mixAttach{ audioProcessor.parameters, Parameters::ID_MIX, mixSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment outGainAttach{ audioProcessor.parameters, Parameters::ID_OUT_GAIN, outGainSlider };

        juce::ToggleButton limiterButton;
        juce::AudioProcessorValueTreeState::ButtonAttachment limiterAttach{ audioProcessor.parameters, Parameters::ID_LIMITER, limiterButton };

		// Distortion
		DriveKnob driveKnob{ audioProcessor, Parameters::ID_DRIVE };

        juce::ComboBox distortionTypeSelector;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> distortionTypeAttach;

        juce::Slider characterSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment characterAttach{ audioProcessor.parameters, Parameters::ID_CHARACTER, characterSlider };

        juce::ComboBox characterTypeSelector;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> characterTypeAttach;

        juce::ToggleButton oversampleButton;
        juce::AudioProcessorValueTreeState::ButtonAttachment oversampleAttach{ audioProcessor.parameters, Parameters::ID_OVERSAMPLE, oversampleButton };

        juce::ToggleButton softClipButton;
        juce::AudioProcessorValueTreeState::ButtonAttachment softClipAttach{ audioProcessor.parameters, Parameters::ID_SOFTCLIP, softClipButton };

        // Feedback
        juce::Slider feedbackSlider, feedbackDelaySlider;
        juce::AudioProcessorValueTreeState::SliderAttachment feedbackAttach{ audioProcessor.parameters, Parameters::ID_FEEDBACK, feedbackSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment feedbackDelayAttach{ audioProcessor.parameters, Parameters::ID_FEEDBACK_DELAY, feedbackDelaySlider };

        // Filter
		FilterComponent filterComponent;

        // Envelope + LFO
        juce::Slider attackSlider, decaySlider, gateSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment attackAttach{ audioProcessor.parameters, Parameters::ID_ENV_ATTACK, attackSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment decayAttach{ audioProcessor.parameters, Parameters::ID_ENV_DECAY, decaySlider };
        juce::AudioProcessorValueTreeState::SliderAttachment gateAttach{ audioProcessor.parameters, Parameters::ID_ENV_GATE, gateSlider };

        SwitchButton envLFOToggleButton{ "Env LFO Toggle", 2, DOWN };

        juce::Slider lfoSpeedSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment lfoSpeedAttach{ audioProcessor.parameters, Parameters::ID_LFO_SPEED, lfoSpeedSlider };

        juce::ToggleButton bypassButton;
        juce::AudioProcessorValueTreeState::ButtonAttachment bypassAttach{ audioProcessor.parameters, Parameters::ID_BYPASS, bypassButton };

        bool showingEnvelope = true;

        // Bounds
        float modParamNamesFontSize = 14.0f;
        juce::Rectangle<float> modParamNamesBox{ 15.0f, 760.0f, 205.0f, 25.0f };

        juce::StringArray modParamNames{"---", "---", "---"};

        juce::StringArray modParamNamesEnvelope{ "Attack", "Decay", "Gate" };
        juce::StringArray modParamNamesLFO{ "Speed", "---", "---" };

        juce::TextButton saveButton;

        juce::TextButton randomizeButton;

        juce::TextButton settingsButton;

        juce::ComboBox presetSelector;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IgnitiveAudioProcessorEditor)

    public:
        IgnitiveAudioProcessorEditor (IgnitiveAudioProcessor&);
        ~IgnitiveAudioProcessorEditor() override;

        void paint (juce::Graphics&) override;
        void resized() override;

};

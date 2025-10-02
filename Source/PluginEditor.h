#pragma once

#include <JuceHeader.h>
#include "Globals.h"
#include "PluginProcessor.h"
#include "FilterCurve.h"
#include "CustomLAFs.h"
#include "IgnitiveLAF.h"
#include "SwitchButtonAttachment.h"
#include "ModMatrixComponent.h"
#include "DriveKnob.h"
#include "Parameters.h"
#include "LevelMeter.h"
#include "ParametersDisplay.h"
#include "ModSourceGraph.h"

class IgnitiveAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer, public juce::AudioProcessorValueTreeState::Listener {
    private:
        IgnitiveAudioProcessor& audioProcessor;

        bool showingEnvelope = true;

        juce::TooltipWindow tooltipWindow{ this, Globals::tooltipDelay };

        // =============== [ RESOURCES ] =============== //

        // Fonts
        juce::Font digitalFont;
        juce::Font uavosdFont;

        // Images
        juce::Image backgroundImage;

        // =============== [ LOOK AND FEELS ] =============== //
        IgnitiveLAF ignitiveLAF{ uavosdFont, digitalFont };
		SwitchLAF switchLAF;
        MixLAF mixLAF;
        BirdsEyeLAF birdsEyeLAF;

        // =============== [ HEADER ] =============== //

        juce::ImageButton saveButton;
        juce::ImageButton randomizeButton;
        juce::ImageButton settingsButton;
        juce::ComboBox presetSelector;
        juce::ToggleButton bypassButton;
        juce::AudioProcessorValueTreeState::ButtonAttachment bypassAttach{ audioProcessor.parameters, Parameters::ID_BYPASS, bypassButton };

        // =============== [ MAIN PANEL ] =============== //
        
        // Filter
        juce::Slider lpCutoffKnob, lpResonanceKnob;
        juce::Slider hpCutoffKnob, hpResonanceKnob;

        juce::AudioProcessorValueTreeState::SliderAttachment lpCutoffAttach{ audioProcessor.parameters, Parameters::ID_LP_CUTOFF, lpCutoffKnob };
        juce::AudioProcessorValueTreeState::SliderAttachment lpResonanceAttach{ audioProcessor.parameters, Parameters::ID_LP_RESONANCE, lpResonanceKnob };
        juce::AudioProcessorValueTreeState::SliderAttachment hpCutoffAttach{ audioProcessor.parameters, Parameters::ID_HP_CUTOFF, hpCutoffKnob };
        juce::AudioProcessorValueTreeState::SliderAttachment hpResonanceAttach{ audioProcessor.parameters, Parameters::ID_HP_RESONANCE, hpResonanceKnob };

        FilterCurve filterCurve;

        // Distortion
        DriveKnob driveKnob{ audioProcessor, Parameters::ID_DRIVE };
        juce::ComboBox distortionTypeSelector;

        juce::Slider characterSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment characterAttach{ audioProcessor.parameters, Parameters::ID_CHARACTER, characterSlider };

        juce::ComboBox characterTypeSelector;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> characterTypeAttach;

        SwitchButton characterPolarityButton;
        SwitchButtonAttachment characterPolarityAttach{ audioProcessor.parameters, Parameters::ID_CHARACTER_POLARITY, characterPolarityButton };

        // Feedback
        juce::Slider feedbackSlider, feedbackDelaySlider;
        juce::AudioProcessorValueTreeState::SliderAttachment feedbackAttach{ audioProcessor.parameters, Parameters::ID_FEEDBACK, feedbackSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment feedbackDelayAttach{ audioProcessor.parameters, Parameters::ID_FEEDBACK_DELAY, feedbackDelaySlider };

        // =============== [ GAIN PANEL ] =============== //
        
        // Gain
        juce::Slider inGainSlider, mixSlider, outGainSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment inGainAttach{ audioProcessor.parameters, Parameters::ID_IN_GAIN, inGainSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment mixAttach{ audioProcessor.parameters, Parameters::ID_MIX, mixSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment outGainAttach{ audioProcessor.parameters, Parameters::ID_OUT_GAIN, outGainSlider };
        LevelMeter inMeter, outMeter;

        juce::ToggleButton limiterButton, oversampleButton;
        juce::AudioProcessorValueTreeState::ButtonAttachment limiterAttach{ audioProcessor.parameters, Parameters::ID_LIMITER, limiterButton };
        juce::AudioProcessorValueTreeState::ButtonAttachment oversampleAttach{ audioProcessor.parameters, Parameters::ID_OVERSAMPLE, oversampleButton };

        // =============== [ MODULATION PANEL ] =============== //

        // Mod Matrix
        juce::Viewport modMatrixViewport;
        ModMatrixComponent modMatrixComponent;

        // Mod Source
        ModSourceGraph modSourceGraph;
        ParametersDisplay paramsDisplay;

        juce::ToggleButton envLFOToggleButton{ "Envelope / LFO" };

        // Envelope
        juce::Slider attackSlider, decaySlider, gateSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment attackAttach{ audioProcessor.parameters, Parameters::ID_ENV_ATTACK, attackSlider };
        juce::AudioProcessorValueTreeState::SliderAttachment decayAttach{ audioProcessor.parameters, Parameters::ID_ENV_DECAY, decaySlider };
        juce::AudioProcessorValueTreeState::SliderAttachment gateAttach{ audioProcessor.parameters, Parameters::ID_ENV_GATE, gateSlider };

        // LFO
        juce::Slider lfoSpeedSlider;
        juce::AudioProcessorValueTreeState::SliderAttachment lfoSpeedAttach{ audioProcessor.parameters, Parameters::ID_LFO_SPEED, lfoSpeedSlider };

		void parameterChanged(const juce::String& parameterID, float newValue) override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IgnitiveAudioProcessorEditor)

    public:
        IgnitiveAudioProcessorEditor (IgnitiveAudioProcessor&);
        ~IgnitiveAudioProcessorEditor() override;

        void paint (juce::Graphics&) override;

        void timerCallback() override;

        void resized() override;
};

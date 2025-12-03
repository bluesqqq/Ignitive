#pragma once

#include <JuceHeader.h>

/*
	A big collection of all the global constants I use in the plugin.
*/
namespace Globals {
	constexpr int pixelSize = 5;
	constexpr int frameRate = 60;

    // The height of unengaged buttons
    constexpr int buttonHeight = 3;

	// The depth of pressed toggle buttons
    constexpr int buttonDepth = 2;

	constexpr float rotaryStartAngle = juce::MathConstants<float>::pi * 1.25f;
	constexpr float rotaryEndAngle = juce::MathConstants<float>::pi * 2.75f;

    // ============== Bounds =============== //
    
	constexpr int windowWidth = 480;
	constexpr int windowHeight = 800;

    // Header
    inline const juce::Rectangle<int> randomizeButtonBounds{ 95,  10 - buttonHeight, 25, 25 + buttonHeight };
    inline const juce::Rectangle<int> saveButtonBounds{ 130, 10 - buttonHeight, 25, 25 + buttonHeight };
    inline const juce::Rectangle<int> presetSelectorBounds{ 165, 10, 150, 25 };
    inline const juce::Rectangle<int> settingsButtonBounds{ 410, 10 - buttonHeight, 25, 25 + buttonHeight };
    inline const juce::Rectangle<int> bypassButtonBounds{ 325, 10 - buttonHeight, 75, 25 + buttonHeight };

    // Filter
    inline const juce::Rectangle<int> hpCutoffKnobBounds{ 40, 95, 60, 60 };
    inline const juce::Rectangle<int> hpResonanceKnobBounds{ 40, 185, 40, 40 };
    inline const juce::Rectangle<int> lpCutoffKnobBounds{ 380, 95, 60, 60 };
    inline const juce::Rectangle<int> lpResonanceKnobBounds{ 400, 185, 40, 40 };
    inline const juce::Rectangle<int> filterCurveBounds{ 110, 85, 260, 80 };

    // Distortion
    inline const juce::Rectangle<int> driveKnobBounds{ 140, 185, 200, 200 };
    inline const juce::Rectangle<int> characterSliderBounds{ 97, 368, 60, 60 };
    inline const juce::Rectangle<int> characterTypeSelectorBounds{ 7, 265, 112, 40 };
    inline const juce::Rectangle<int> distortionTypeSelectorBounds{ 361, 265, 112, 40 };
    inline const juce::Rectangle<int> characterPolarityButtonBounds{ 7, 313, 35, 35 };

    // Feedback
    inline const juce::Rectangle<int> feedbackSliderBounds{ 354, 348, 80, 80 };
    inline const juce::Rectangle<int> feedbackDelaySliderBounds{ 296, 417, 40, 40 };

    // Gain Panel
    inline const juce::Rectangle<int> inGainSliderBounds{ 10, 535, 60, 60 };
    inline const juce::Rectangle<int> inMeterBounds{ 62, 587, 14, 14 };
    inline const juce::Rectangle<int> oversampleButtonBounds{ 80, 535 - buttonHeight, 110, 25 + buttonHeight };
    inline const juce::Rectangle<int> limiterButtonBounds{ 80, 570 - buttonHeight, 110, 25 + buttonHeight };
    inline const juce::Rectangle<int> outGainSliderBounds{ 200, 535, 60, 60 };
    inline const juce::Rectangle<int> outMeterBounds{ 252, 587, 14, 14 };
    inline const juce::Rectangle<int> mixSliderBounds{ 275, 540, 190, 50 };

    // Mod Panel
    inline const juce::Rectangle<int> modMatrixViewportBounds{ 280, 625, 180, 155 };
    inline const juce::Rectangle<int> modMatrixComponentSize{ 165, 250 };
    inline const juce::Rectangle<int> envLFOToggleButtonBounds{ 237, 640, 20, 40 };
    inline const juce::Rectangle<int> attackSliderBounds{ 30, 710, 40, 40 };
    inline const juce::Rectangle<int> decaySliderBounds{ 97, 710, 40, 40 };
    inline const juce::Rectangle<int> gateSliderBounds{ 165, 710, 40, 40 };
    inline const juce::Rectangle<int> lfoSpeedSliderBounds{ 30, 710, 40, 40 };
    inline const juce::Rectangle<int> modSourceGraphBounds{ 15, 620, 205, 80 };
    inline const juce::Rectangle<int> paramsDisplayBounds{ 15, 760, 205, 25 };

	// ============== Colours =============== //
    inline juce::Colour distortionColor{ 0xFFFF0000 };
    inline juce::Colour filterColor{ 0xFFFFa500 };
    inline juce::Colour envelopeColour{ 0xFFFFFF00 };
    inline juce::Colour lfoColour{ 0xFFFFa500 };

    constexpr float dimAmount = 0.9f;

    // ============== Tooltips =============== //
	constexpr int tooltipDelay = 700; // milliseconds

    inline constexpr const char* tooltipLowpassCutoff = "Lowpass filter cutoff frequency";
    inline constexpr const char* tooltipLowpassResonance = "Lowpass filter resonance";
    inline constexpr const char* tooltipHighpassCutoff = "Highpass filter cutoff frequency";
    inline constexpr const char* tooltipHighpassResonance = "Highpass filter resonance";
    inline constexpr const char* tooltipInGain = "In gain";
    inline constexpr const char* tooltipOutGain = "Out gain";
    inline constexpr const char* tooltipMix = "Dry/wet mix";
    inline constexpr const char* tooltipOversample = "Toggle distortion oversampling on/off";
    inline constexpr const char* tooltipLimiter = "Toggle limiter on/off";
    inline constexpr const char* tooltipDistortionType = "Distortion type";
    inline constexpr const char* tooltipDistortionCharacter = "Distortion character amount";
    inline constexpr const char* tooltipDistortionCharType = "Distortion character type";
    inline constexpr const char* tooltipDistortionCharPol = "Distortion character polarity";
    inline constexpr const char* tooltipFeedback = "Feedback amount";
    inline constexpr const char* tooltipFeedbackDelay = "Feedback delay time";
    inline constexpr const char* tooltipEnvAttack = "Envelope attack time";
    inline constexpr const char* tooltipEnvDecay = "Envelope decay time";
    inline constexpr const char* tooltipEnvGate = "Envelope gate threshold";
    inline constexpr const char* tooltipLfoSpeed = "LFO speed";
    inline constexpr const char* tooltipEnvLfoToggle = "Toggle envelope/lfo display";
    inline constexpr const char* tooltipRandomize = "Randomize parameters";
    inline constexpr const char* tooltipSettings = "Open settings panel";
    inline constexpr const char* tooltipSavePreset = "Save current patch as preset";
    inline constexpr const char* tooltipPresetSelector = "Select preset";
	inline constexpr const char* tooltipBypass = "Toggle bypass on/off";
}
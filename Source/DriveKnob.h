#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLAFs.h"

class DriveKnob : public juce::Component, private juce::Timer {
	private:
		DriveLAF driveLAF;

		juce::Slider driveSlider;
		juce::AudioProcessorValueTreeState::SliderAttachment driveAttach;

	public:
		DriveKnob(IgnitiveAudioProcessor& processor, const juce::String& parameterID) 
			: driveLAF(processor.getDistortionEngine()), driveAttach(processor.apvts, parameterID, driveSlider) {
			startTimer(1000 / 60);
			driveSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
			driveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
			driveSlider.setLookAndFeel(&driveLAF);
			addAndMakeVisible(driveSlider);
		}

		void resized() override {
			driveSlider.setBounds(getLocalBounds());
		}

		void timerCallback() override {
			repaint();
		}
};
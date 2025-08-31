#include "FilterComponent.h"

FilterComponent::FilterComponent(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix)
    : cutoffAttach(apvts, prefix + "Cutoff", cutoffKnob),
    resonanceAttach(apvts, prefix + "Resonance", resonanceKnob),
    typeAttach(apvts, prefix + "Type", typeButton),
    toggleAttach(apvts, prefix + "FilterOn", toggleButton) {

    cutoffKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    cutoffKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(cutoffKnob);

    resonanceKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    resonanceKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(resonanceKnob);

    addAndMakeVisible(typeButton);
    addAndMakeVisible(toggleButton);
}

void FilterComponent::resized() {
    cutoffKnob.setBounds(25, 125, 50, 50);
    resonanceKnob.setBounds(25, 200, 50, 50);
    typeButton.setBounds(25, 275, 50, 50);
    toggleButton.setBounds(25, 350, 50, 50);
}
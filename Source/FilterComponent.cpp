#include "FilterComponent.h"

FilterComponent::FilterComponent(juce::AudioProcessorValueTreeState& params, FilterProcessor& filter)
    : cutoffAttach(params, filter.cutoffID, cutoffKnob),
    resonanceAttach(params, filter.resonanceID, resonanceKnob),
    typeAttach(params, filter.typeID, typeButton),
    enabledAttach(params, filter.enabledID, enabledButton) {

    cutoffKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    cutoffKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    cutoffKnob.setLookAndFeel(&knobLAF);
    cutoffKnob.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(cutoffKnob);

    resonanceKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    resonanceKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    resonanceKnob.setLookAndFeel(&knobLAF);
    resonanceKnob.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(resonanceKnob);

    addAndMakeVisible(typeButton);
    addAndMakeVisible(enabledButton);
}

void FilterComponent::resized() {
    cutoffKnob.setBounds(20, 215, 60, 60);
    resonanceKnob.setBounds(86, 280, 40, 40);
    typeButton.setBounds(50, 290, 20, 60);
    enabledButton.setBounds(25, 375, 50, 50);
}
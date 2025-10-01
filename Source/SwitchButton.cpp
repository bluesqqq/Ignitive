#include "SwitchButton.h"

SwitchButton::SwitchButton(const juce::String& buttonName, const juce::Font& font)
    : juce::Button(buttonName), font(font), index(0) {}

void SwitchButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
    auto bounds = getLocalBounds().toFloat();

    juce::String text = "";

    switch(index) {
        case 0: text = "+"; break;
        case 1: text = "-"; break;
        case 2: text = "+/-"; break;
	}

	g.setFont(font.withHeight(20.0f));
	g.setColour(juce::Colours::red);
    g.drawText(text, bounds, juce::Justification::centred);
}

void SwitchButton::clicked() { setIndex(index + 1); }

unsigned int SwitchButton::getIndex() { return index; }

unsigned int SwitchButton::getNumStates() { return numStates; }

void SwitchButton::setIndex(int newIndex) { 
    index = newIndex % numStates; 
    repaint(); 
	if (onIndexChange) onIndexChange(index);
}
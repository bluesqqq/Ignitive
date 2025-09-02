/*
  ==============================================================================

    SwitchButton.cpp
    Created: 1 Sep 2025 11:18:02am
    Author:  blues

  ==============================================================================
*/

#include "SwitchButton.h"

SwitchButton::SwitchButton(const juce::String& buttonName, unsigned int numberOfStates, Direction direction)
    : juce::Button(buttonName), numStates(numberOfStates), index(0), direction(direction) {}

void SwitchButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
    // Easier and more discrete than making a LookAndFeel
    g.setColour(juce::Colours::black);
    g.fillAll();

    auto bounds = getLocalBounds();

    int border = 3;

    int x = bounds.getX();
    int y = bounds.getY();
    int w = bounds.getWidth();
    int h = bounds.getHeight();

	switch (direction) {
	    case UP: {
        case DOWN:
            int increment = (h - border * 2) / numStates;

            g.setColour(juce::Colours::white);
            g.fillRect(x + border, y + border + (index * increment), w - (border * 2), increment);
            break;
        }
        case LEFT: {
        case RIGHT:
            int increment = (w - border * 2) / numStates;

            g.setColour(juce::Colours::white);
            g.fillRect(x + border + (index * increment), y + border, increment, h - (border * 2));
            break;
        }
	}
}

void SwitchButton::clicked() { setIndex(index + 1); }

unsigned int SwitchButton::getIndex() { return index; }

unsigned int SwitchButton::getNumStates() { return numStates; }

void SwitchButton::setIndex(int newIndex) { 
    index = newIndex % numStates; 
    repaint(); 
	if (onIndexChange) onIndexChange(index);
}
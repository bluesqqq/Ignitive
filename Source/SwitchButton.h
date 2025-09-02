#pragma once

#include <JuceHeader.h>

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class SwitchButton : public juce::Button {
    private:
        unsigned int index;
        unsigned int numStates;
        Direction direction;

    public:
        std::function<void(int)> onIndexChange;

        SwitchButton(const juce::String& buttonName, unsigned int numberOfStates = 1, Direction direction = RIGHT);

        void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

        void clicked() override;

        unsigned int getIndex();

        unsigned int getNumStates();

        void setIndex(int newIndex);
};


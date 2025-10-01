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
        unsigned int index = 0;
        unsigned int numStates = 3;
        Direction direction = RIGHT;

        juce::Font font;

    public:
        std::function<void(int)> onIndexChange;

        SwitchButton(const juce::String& buttonName, const juce::Font& font);

        void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

        void clicked() override;

        unsigned int getIndex();

        unsigned int getNumStates();

        void setIndex(int newIndex);
};


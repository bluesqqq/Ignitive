#pragma once

#include <JuceHeader.h>
#include "SwitchButtonAttachment.h"
#include "CustomLAFs.h"
#include "FilterProcessor.h"

class FilterComponent : public juce::Component {
    private:
		KnobLAF knobLAF;

        juce::Slider cutoffKnob, resonanceKnob;
        SwitchButton typeButton{ "Filter Type", 3, DOWN };
        juce::ToggleButton enabledButton;

        juce::AudioProcessorValueTreeState::SliderAttachment cutoffAttach, resonanceAttach;
        SwitchButtonAttachment typeAttach;
        juce::AudioProcessorValueTreeState::ButtonAttachment enabledAttach;

    public:
        FilterComponent(juce::AudioProcessorValueTreeState& parameters, FilterProcessor& filter);

        void resized() override;
};
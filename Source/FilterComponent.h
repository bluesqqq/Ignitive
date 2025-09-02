#pragma once

#include <JuceHeader.h>
#include "SwitchButtonAttachment.h"
#include "CustomLAFs.h"

class FilterComponent : public juce::Component {
    private:
		KnobLAF knobLAF;

        juce::Slider cutoffKnob, resonanceKnob;
        SwitchButton typeButton{ "Filter Type", 3, DOWN };
        juce::ToggleButton toggleButton;

        juce::AudioProcessorValueTreeState::SliderAttachment cutoffAttach, resonanceAttach;
        SwitchButtonAttachment typeAttach;
        juce::AudioProcessorValueTreeState::ButtonAttachment toggleAttach;

    public:
        FilterComponent(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix);

        void resized() override;
};
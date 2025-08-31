#pragma once

#include <JuceHeader.h>

class FilterComponent : public juce::Component {
    private:
        juce::Slider cutoffKnob, resonanceKnob;
        juce::AudioProcessorValueTreeState::SliderAttachment cutoffAttach, resonanceAttach;

        juce::TextButton typeButton;
        juce::AudioProcessorValueTreeState::ButtonAttachment typeAttach;

        juce::ToggleButton toggleButton;
        juce::AudioProcessorValueTreeState::ButtonAttachment toggleAttach;

    public:
        FilterComponent(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix);

        void resized() override;
};
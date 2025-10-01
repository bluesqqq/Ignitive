#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ModSource.h"

class ModSourceGraph : public juce::Component {
    private:
        std::deque<float> values;
        IgnitiveAudioProcessor& audioProcessor;
        ModSource* source = nullptr;

        juce::Colour highlightColor = juce::Colours::orange;
        juce::Colour backgroundColor = juce::Colours::darkorange;

        static constexpr int pixelSize = 5;

    public:
        ModSourceGraph(IgnitiveAudioProcessor& audioProcessor, ModSource* source);

        void setSource(ModSource* source);

        void paint(juce::Graphics& g);
};

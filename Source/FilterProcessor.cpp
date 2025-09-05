
#include "FilterProcessor.h"

FilterProcessor::FilterProcessor(juce::AudioProcessorValueTreeState& params, const juce::String& cutID, const juce::String& resID, const juce::String& typeID, const juce::String& enabledID)
    : parameters(params), cutoffID(cutID), resonanceID(resID), typeID(typeID), enabledID(enabledID) {
}

void FilterProcessor::prepare(const juce::dsp::ProcessSpec& spec) {
    filter.prepare(spec);
}

void FilterProcessor::process(const juce::dsp::ProcessContextReplacing<float>& context) {
    if (!parameters.getRawParameterValue(enabledID)->load()) return; // bypass

    float cutoffNorm = parameters.getRawParameterValue(cutoffID)->load();
    float resonanceNorm = parameters.getRawParameterValue(resonanceID)->load();
    int typeIndex = (int)parameters.getRawParameterValue(typeID)->load();

    float cutoffHz = 20.0f * std::pow(10.0f, cutoffNorm * 3.0f);
    float resonance = juce::jmap(resonanceNorm, 0.707f, 4.0f);

    filter.setType(static_cast<juce::dsp::StateVariableTPTFilterType>(typeIndex));
    filter.setCutoffFrequency(cutoffHz);
    filter.setResonance(resonance);

    filter.process(context);
}

void FilterProcessor::reset() {
    filter.reset();
}
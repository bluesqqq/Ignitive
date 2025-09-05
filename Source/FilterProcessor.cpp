
#include "FilterProcessor.h"

FilterProcessor::FilterProcessor(juce::AudioProcessorValueTreeState& params, const juce::String& cutID, const juce::String& resID, const juce::String& typeID, const juce::String& enabledID)
    : parameters(params), cutoffID(cutID), resonanceID(resID), typeID(typeID), enabledID(enabledID) {
}

void FilterProcessor::prepare(const juce::dsp::ProcessSpec& spec) {
    filter.prepare(spec);

    cutoff.reset(spec.sampleRate, 0.02);
    resonance.reset(spec.sampleRate, 0.02);
}

void FilterProcessor::process(const juce::dsp::ProcessContextReplacing<float>& context) {
    if (!parameters.getRawParameterValue(enabledID)->load()) return; // bypass

    cutoff.setTargetValue(*parameters.getRawParameterValue(cutoffID));
    resonance.setTargetValue(*parameters.getRawParameterValue(resonanceID));

    int typeIndex = (int)parameters.getRawParameterValue(typeID)->load();

    filter.setType(static_cast<juce::dsp::StateVariableTPTFilterType>(typeIndex));

    auto& block = context.getOutputBlock();
    auto numSamples = block.getNumSamples();
    auto numChannels = block.getNumChannels();

    for (size_t sample = 0; sample < numSamples; ++sample) {
        float cutoffNorm = cutoff.getNextValue();
        float resonanceNorm = resonance.getNextValue();

        float cutoffHz = 20.0f * std::pow(10.0f, cutoffNorm * 3.0f);
        float qResonance = juce::jmap(resonanceNorm, 0.707f, 4.0f);

        filter.setCutoffFrequency(cutoffHz);
        filter.setResonance(qResonance);

        for (size_t ch = 0; ch < numChannels; ++ch) {
            float* data = block.getChannelPointer(ch);
            data[sample] = filter.processSample(ch, data[sample]);
        }
    }
}

void FilterProcessor::reset() {
    filter.reset();
}
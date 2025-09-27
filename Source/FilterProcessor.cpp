
#include "FilterProcessor.h"

FilterProcessor::FilterProcessor(juce::AudioProcessorValueTreeState& params, ModMatrix& matrix, const juce::String& lpCutID, const juce::String& lpResID, const juce::String& hpCutID, const juce::String& hpResID)
    : parameters(params), modMatrix(matrix), lpCutoffID(lpCutID), lpResonanceID(lpResID), hpCutoffID(hpCutID), hpResonanceID(hpResID) {
}

void FilterProcessor::prepare(const juce::dsp::ProcessSpec& spec) {
    lpFilter.prepare(spec);
    lpFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    hpFilter.prepare(spec);
    hpFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
}

void FilterProcessor::process(const juce::dsp::ProcessContextReplacing<float>& context) {
    auto& block = context.getOutputBlock();
    auto numSamples = block.getNumSamples();
    auto numChannels = block.getNumChannels();

    ModDestination* lpCutDest = modMatrix.getDestination(lpCutoffID);
    ModDestination* lpResDest = modMatrix.getDestination(lpResonanceID);
    ModDestination* hpCutDest = modMatrix.getDestination(hpCutoffID);
    ModDestination* hpResDest = modMatrix.getDestination(hpResonanceID);
    for (size_t sample = 0; sample < numSamples; ++sample) {
        float lpCutoff    = lpCutDest->getValue(sample);
        float lpResonance = lpResDest->getValue(sample);
        float hpCutoff    = hpCutDest->getValue(sample);
        float hpResonance = hpResDest->getValue(sample);

        float lpCutoffHz = 20.0f * std::pow(10.0f, lpCutoff * 3.0f);
        float lpResonanceQ = juce::jmap(lpResonance, 0.707f, 4.0f);
        float hpCutoffHz = 20.0f * std::pow(10.0f, hpCutoff * 3.0f);
        float hpResonanceQ = juce::jmap(hpResonance, 0.707f, 4.0f);

        lpFilter.setCutoffFrequency(juce::jlimit(0.0f, 20000.0f, lpCutoffHz));
        lpFilter.setResonance(lpResonanceQ);
        hpFilter.setCutoffFrequency(juce::jlimit(0.0f, 20000.0f, hpCutoffHz));
        hpFilter.setResonance(hpResonanceQ);

        for (size_t channel = 0; channel < numChannels; ++channel) {
            float* data = block.getChannelPointer(channel);
            data[sample] = hpFilter.processSample(channel, lpFilter.processSample(channel, data[sample]));
        }
    }
}

void FilterProcessor::reset() {
    lpFilter.reset();
    hpFilter.reset();
}

void FilterProcessor::processBlockSample(juce::dsp::AudioBlock<float>& block, size_t sample) {
    float lpCutoff    = modMatrix.getValue(lpCutoffID, sample);
    float lpResonance = modMatrix.getValue(lpResonanceID, sample);
    float hpCutoff    = modMatrix.getValue(hpCutoffID, sample);
    float hpResonance = modMatrix.getValue(hpResonanceID, sample);

    float lpCutoffHz = 20.0f * std::pow(10.0f, lpCutoff * 3.0f);
    float lpResonanceQ = juce::jmap(lpResonance, 0.707f, 4.0f);
    float hpCutoffHz = 20.0f * std::pow(10.0f, hpCutoff * 3.0f);
    float hpResonanceQ = juce::jmap(hpResonance, 0.707f, 4.0f);

    lpFilter.setCutoffFrequency(lpCutoffHz);
    lpFilter.setResonance(lpResonanceQ);
    hpFilter.setCutoffFrequency(hpCutoffHz);
    hpFilter.setResonance(hpResonanceQ);

    for (size_t channel = 0; channel < block.getNumChannels(); ++channel) {
        float* data = block.getChannelPointer(channel);
        data[sample] = lpFilter.processSample(channel, data[sample]);
        data[sample] = hpFilter.processSample(channel, data[sample]);
    }
}
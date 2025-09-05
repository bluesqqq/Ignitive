#include "IgnitiveEngine.h"
#include "Parameters.h"

IgnitiveEngine::IgnitiveEngine(juce::AudioProcessorValueTreeState& params, juce::AudioProcessor& p)
	: parameters(params), processor(p),
	  distortion(parameters), feedback(parameters), 
      preFilter(parameters,  Parameters::ID_PRE_FILTER_CUTOFF,  Parameters::ID_PRE_FILTER_RESONANCE,  Parameters::ID_PRE_FILTER_TYPE,  Parameters::ID_PRE_FILTER_ENABLED),
      postFilter(parameters, Parameters::ID_POST_FILTER_CUTOFF, Parameters::ID_POST_FILTER_RESONANCE, Parameters::ID_POST_FILTER_TYPE, Parameters::ID_POST_FILTER_ENABLED) {
}

void IgnitiveEngine::prepare(const juce::dsp::ProcessSpec& spec) {
	distortion.prepare(spec);
	feedback.prepare(spec);
	preFilter.prepare(spec);
	postFilter.prepare(spec);

    inGain.reset(spec.sampleRate, 0.02);
    outGain.reset(spec.sampleRate, 0.02);
}

void IgnitiveEngine::process(const juce::dsp::ProcessContextReplacing<float>& context) {
    auto& block = context.getOutputBlock();
    auto numSamples = block.getNumSamples();
    auto numChannels = block.getNumChannels();

    inGain.setTargetValue(*parameters.getRawParameterValue(Parameters::ID_IN_GAIN));
    outGain.setTargetValue(*parameters.getRawParameterValue(Parameters::ID_OUT_GAIN));

    // Input Gain
    for (size_t sample = 0; sample < numSamples; ++sample) {
        auto g = inGain.getNextValue();
        for (size_t ch = 0; ch < numChannels; ++ch)
            block.getChannelPointer(ch)[sample] *= g;
    }

    // Envelope processing here

    // DSP
    preFilter.process(context);
    distortion.process(context);
    feedback.process(context);
    postFilter.process(context);

    // Output gain
    for (size_t sample = 0; sample < numSamples; ++sample) {
        auto g = outGain.getNextValue();
        for (size_t ch = 0; ch < numChannels; ++ch)
            block.getChannelPointer(ch)[sample] *= g;
    }
}


void IgnitiveEngine::reset() {
    distortion.reset();
    feedback.reset();
    preFilter.reset();
    postFilter.reset();
}

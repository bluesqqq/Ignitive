#include "IgnitiveEngine.h"
#include "Parameters.h"

IgnitiveEngine::IgnitiveEngine(juce::AudioProcessorValueTreeState& params, juce::AudioProcessor& p)
	: parameters(params), processor(p), modMatrix(params), 
	  distortion(parameters, modMatrix, Parameters::ID_DRIVE, Parameters::ID_COLOR, Parameters::ID_DISTORTION_TYPE),
      feedback  (parameters, modMatrix, Parameters::ID_FEEDBACK, Parameters::ID_FEEDBACK_DELAY),
      preFilter (parameters, modMatrix, Parameters::ID_PRE_FILTER_CUTOFF,  Parameters::ID_PRE_FILTER_RESONANCE,  Parameters::ID_PRE_FILTER_TYPE,  Parameters::ID_PRE_FILTER_ENABLED),
      postFilter(parameters, modMatrix, Parameters::ID_POST_FILTER_CUTOFF, Parameters::ID_POST_FILTER_RESONANCE, Parameters::ID_POST_FILTER_TYPE, Parameters::ID_POST_FILTER_ENABLED) {

    for (const auto& id : Parameters::destinationIDs) {
        modMatrix.addDestination(id, params);
    }

    modMatrix.addSource(Parameters::ID_ENV, &envelope);

    modMatrix.makeConnection(Parameters::ID_ENV, Parameters::ID_DRIVE, 1.0f);
}

void IgnitiveEngine::prepare(const juce::dsp::ProcessSpec& spec) {
	distortion.prepare(spec);
	feedback.prepare(spec);
	preFilter.prepare(spec);
	postFilter.prepare(spec);

    envelope.prepare(spec);

    modMatrix.prepare(spec);

    inGain.reset(spec.sampleRate, 0.02);
    outGain.reset(spec.sampleRate, 0.02);
}

void IgnitiveEngine::process(const juce::dsp::ProcessContextReplacing<float>& context) {
    auto& block = context.getOutputBlock();
    auto numSamples = block.getNumSamples();
    auto numChannels = block.getNumChannels();

    inGain.setTargetValue(*parameters.getRawParameterValue(Parameters::ID_IN_GAIN));
    outGain.setTargetValue(*parameters.getRawParameterValue(Parameters::ID_OUT_GAIN));

    /*
      Chain Layout:

      -> IN GAIN -> PRE FILTER -> DISTORTION -> POST FILTER -> [FEEDBACK] -> MIX -> OUT GAIN ->
                                             ^                     │
                                             └─────────────────────┘

      Feedback is written after the post filter and mixed back in after a delay before the post filter
    */

    // Input Gain
    for (size_t sample = 0; sample < numSamples; ++sample) {
        auto g = inGain.getNextValue();
        for (size_t ch = 0; ch < numChannels; ++ch)
            block.getChannelPointer(ch)[sample] *= g;
    }

    // Envelope processing
    envelope.setAttackTime(*parameters.getRawParameterValue(Parameters::ID_ENV_ATTACK));
    envelope.setReleaseTime(*parameters.getRawParameterValue(Parameters::ID_ENV_DECAY));
    envelope.setGate(*parameters.getRawParameterValue(Parameters::ID_ENV_GATE));
    envelope.process(block);

    modMatrix.update();

    // DSP
    preFilter.process(context);
    distortion.process(context);
    
    postFilter.updateParameters();

    // Post filter has to be processed per sample due to feedback's ordering
    // See graph above for visual explanation
    for (size_t sample = 0; sample < numSamples; ++sample) {
        feedback.processBlockSample(block, sample);
        postFilter.processBlockSample(block, sample);
        feedback.processWriteBlockSample(block, sample);
    }


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

#include "IgnitiveEngine.h"
#include "Parameters.h"

IgnitiveEngine::IgnitiveEngine(juce::AudioProcessorValueTreeState& params, juce::AudioProcessor& p)
	: parameters(params), processor(p), modMatrix(params), 
	  distortion(parameters, modMatrix, Parameters::ID_DRIVE, Parameters::ID_CHARACTER, Parameters::ID_DISTORTION_TYPE, Parameters::ID_CHARACTER_TYPE),
      feedback  (parameters, modMatrix, Parameters::ID_FEEDBACK, Parameters::ID_FEEDBACK_DELAY),
      filter (parameters, modMatrix, Parameters::ID_LP_CUTOFF, Parameters::ID_LP_RESONANCE, Parameters::ID_HP_CUTOFF, Parameters::ID_HP_RESONANCE) {

    modMatrix.addDestination(Parameters::ID_DRIVE, "Drive", params);
    modMatrix.addDestination(Parameters::ID_CHARACTER, "Color", params);
    modMatrix.addDestination(Parameters::ID_FEEDBACK, "Feedback", params);
    modMatrix.addDestination(Parameters::ID_FEEDBACK_DELAY, "Delay", params);


    modMatrix.addDestination(Parameters::ID_LP_CUTOFF,    "LP Cut", params);
    modMatrix.addDestination(Parameters::ID_LP_RESONANCE, "LP Res", params);
    modMatrix.addDestination(Parameters::ID_HP_CUTOFF,    "HP Cut", params);
    modMatrix.addDestination(Parameters::ID_HP_RESONANCE, "HP Res", params);

    modMatrix.addSource(Parameters::ID_ENV, &envelope);

    for (int i = 0; i < 8; i++)
        modMatrix.makeConnection(Parameters::ID_ENV, "", 0.0f);
}

void IgnitiveEngine::prepare(const juce::dsp::ProcessSpec& spec) {
	distortion.prepare(spec);
	feedback.prepare(spec);
	filter.prepare(spec);

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
    filter.process(context);
    distortion.process(context);

    // Post filter has to be processed per sample due to feedback's ordering
    // See graph above for visual explanation
    for (size_t sample = 0; sample < numSamples; ++sample) {
        feedback.processBlockSample(block, sample);
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
    filter.reset();
}

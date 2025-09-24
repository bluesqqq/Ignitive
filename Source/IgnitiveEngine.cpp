#include "IgnitiveEngine.h"
#include "Parameters.h"

IgnitiveEngine::IgnitiveEngine(juce::AudioProcessorValueTreeState& params, juce::AudioProcessor& p)
	: parameters(params), processor(p), modMatrix(params), 
	  distortion(parameters, modMatrix, Parameters::ID_DRIVE, Parameters::ID_CHARACTER, Parameters::ID_DISTORTION_TYPE, Parameters::ID_CHARACTER_TYPE, Parameters::ID_OVERSAMPLE),
      feedback  (parameters, modMatrix, Parameters::ID_FEEDBACK, Parameters::ID_FEEDBACK_DELAY),
      filter (parameters, modMatrix, Parameters::ID_LP_CUTOFF, Parameters::ID_LP_RESONANCE, Parameters::ID_HP_CUTOFF, Parameters::ID_HP_RESONANCE),
      inGain(parameters, Parameters::ID_IN_GAIN), outGain(parameters, Parameters::ID_OUT_GAIN), 
      lfo(parameters, Parameters::ID_LFO_SPEED) {

    modMatrix.addDestination(Parameters::ID_DRIVE, "Drive", params);
    modMatrix.addDestination(Parameters::ID_CHARACTER, "Character", params);
    modMatrix.addDestination(Parameters::ID_FEEDBACK, "Feedback", params);
    modMatrix.addDestination(Parameters::ID_FEEDBACK_DELAY, "Delay", params);


    modMatrix.addDestination(Parameters::ID_LP_CUTOFF,    "LP Cut", params);
    modMatrix.addDestination(Parameters::ID_LP_RESONANCE, "LP Res", params);
    modMatrix.addDestination(Parameters::ID_HP_CUTOFF,    "HP Cut", params);
    modMatrix.addDestination(Parameters::ID_HP_RESONANCE, "HP Res", params);

    modMatrix.addSource(Parameters::ID_ENV, &envelope);
    modMatrix.addSource(Parameters::ID_LFO, &lfo);

    for (int i = 0; i < 8; i++) {
        modMatrix.makeConnection(Parameters::ID_ENV, "", 0.0f);
    }

    for (int i = 0; i < 8; i++) {
        modMatrix.makeConnection(Parameters::ID_LFO, "", 0.0f);
    }
}

void IgnitiveEngine::prepare(const juce::dsp::ProcessSpec& spec) {
    distortion.prepare(spec);
    feedback.prepare(spec);
    filter.prepare(spec);
    envelope.prepare(spec);
    lfo.prepare(spec);
    modMatrix.prepare(spec);
}

void IgnitiveEngine::process(const juce::dsp::ProcessContextReplacing<float>& context) {
    bool bypass = parameters.getRawParameterValue(Parameters::ID_BYPASS)->load();

    if (!bypass) {
        auto& block = context.getOutputBlock();
        auto numSamples = block.getNumSamples();
        auto numChannels = block.getNumChannels();

        bool limiterEnabled = parameters.getRawParameterValue(Parameters::ID_LIMITER)->load();
        bool softClip = parameters.getRawParameterValue(Parameters::ID_SOFTCLIP)->load();
        float mix = parameters.getRawParameterValue(Parameters::ID_MIX)->load();

        // Store the dry (unprocessed) signal
        juce::AudioBuffer<float> dryBuffer(numChannels, numSamples);
        for (size_t ch = 0; ch < numChannels; ++ch) {
            dryBuffer.copyFrom(ch, 0, block.getChannelPointer(ch), numSamples);
        }

        /*
          Chain Layout:

          -> IN GAIN -> PRE FILTER -> DISTORTION -> POST FILTER -> [FEEDBACK] -> MIX -> OUT GAIN ->
                                                 ^                     │
                                                 └─────────────────────┘

          Feedback is written after the post filter and mixed back in after a delay before the post filter
        */

        inGain.process(context);

        // Envelope processing
        envelope.setAttackTime(*parameters.getRawParameterValue(Parameters::ID_ENV_ATTACK));
        envelope.setReleaseTime(*parameters.getRawParameterValue(Parameters::ID_ENV_DECAY));
        envelope.setGate(*parameters.getRawParameterValue(Parameters::ID_ENV_GATE));
        envelope.process(block);

        lfo.update();

        modMatrix.update();

        // DSP
        filter.process(context);
        distortion.process(context);

        for (size_t sample = 0; sample < numSamples; ++sample) {
            feedback.processBlockSample(block, sample);
            feedback.processWriteBlockSample(block, sample);
        }

        outGain.process(context);

        // Limiter
        if (limiterEnabled) {
            for (size_t ch = 0; ch < numChannels; ++ch) {
                auto* channelData = block.getChannelPointer(ch);
                if (softClip) {
                    for (size_t sample = 0; sample < numSamples; ++sample) {
                        channelData[sample] = std::tanh(channelData[sample]);
                    }
                } else {
                    for (size_t sample = 0; sample < numSamples; ++sample) {
                        channelData[sample] = juce::jlimit(-1.0f, 1.0f, channelData[sample]);
                    }
                }
            }
        }

        // DRY / WET
        for (size_t ch = 0; ch < numChannels; ++ch) {
            auto* wetSignal = block.getChannelPointer(ch);
            const auto* drySignal = dryBuffer.getReadPointer(ch);

            for (size_t sample = 0; sample < numSamples; ++sample) {
                wetSignal[sample] = juce::jmap(mix, drySignal[sample], wetSignal[sample]);
            }
        }
    }
}


void IgnitiveEngine::reset() {
    distortion.reset();
    feedback.reset();
    filter.reset();
}
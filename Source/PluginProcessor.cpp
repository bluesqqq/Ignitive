#include "PluginProcessor.h"
#include "PluginEditor.h"


juce::AudioProcessorValueTreeState::ParameterLayout IgnitiveAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Gain
    layout.add(std::make_unique<juce::AudioParameterFloat>("inGain", "In Gain", 0.0f, 2.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("outGain", "Out Gain", 0.0f, 2.0f, 1.0f));

    // Distortion + Feedback
    layout.add(std::make_unique<juce::AudioParameterFloat>("drive", "Drive", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("color", "Color", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("distortionType", "Distortion Type", juce::StringArray{ "Hard Clip", "Tube", "Fuzz", "Rectify", "Downsample" }, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("feedback", "Feedback", 0.0f, 0.8f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("feedbackDelay", "Feedback Delay", 0.001f, 0.1f, 0.01f));

    // Filters
    layout.add(std::make_unique<juce::AudioParameterFloat>("preCutoff", "Cutoff", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("preResonance", "Resonance", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("preType", "Filter Type", juce::StringArray{ "LP", "BP", "HP"}, 0));
    layout.add(std::make_unique<juce::AudioParameterBool>("preFilterOn", "Filter On", false));

    layout.add(std::make_unique<juce::AudioParameterFloat>("postCutoff", "Cutoff", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("postResonance", "Resonance", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("postType", "Filter Type", juce::StringArray{ "LP", "BP", "HP" }, 0));
    layout.add(std::make_unique<juce::AudioParameterBool>("postFilterOn", "Filter On", false));

    // Envelope
    layout.add(std::make_unique<juce::AudioParameterFloat>("envAttack", "Attack", 0.0f, 0.1f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("envDecay", "Decay", 0.0f, 0.2f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("envGate", "Gate", 0.0f, 1.0f, 0.0f));

    // LFO
	layout.add(std::make_unique<juce::AudioParameterFloat>("lfoRate", "LFO Rate", 0.0f, 20.0f, 0.0f));
	layout.add(std::make_unique<juce::AudioParameterChoice>("lfoWaveform", "LFO Waveform", juce::StringArray{ "Sine", "Triangle", "Square", "Saw" }, 0));

    return layout;
}

IgnitiveAudioProcessor::IgnitiveAudioProcessor()
    : AudioProcessor (BusesProperties().withInput("Input", 
      juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      distortion(DistortionEngine()) {
	modMatrix.addSource(&envelopeFollower);

	modMatrix.addDestination(&driveDest);
	modMatrix.addDestination(&colorDest);
	modMatrix.addDestination(&preCutoffDest);
	modMatrix.addDestination(&preResonanceDest);
	modMatrix.addDestination(&postCutoffDest);
	modMatrix.addDestination(&postResonanceDest);
	modMatrix.addDestination(&feedbackDest);
	modMatrix.addDestination(&feedbackDelayDest);

    modMatrix.makeConnection(0, 0, 0.5f);
}

IgnitiveAudioProcessor::~IgnitiveAudioProcessor() {
}

const juce::String IgnitiveAudioProcessor::getName() const { return JucePlugin_Name; }

bool IgnitiveAudioProcessor::acceptsMidi() const { return false; }

bool IgnitiveAudioProcessor::producesMidi() const { return false; }

bool IgnitiveAudioProcessor::isMidiEffect() const { return false; }

double IgnitiveAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int IgnitiveAudioProcessor::getNumPrograms() { return 1; }

int IgnitiveAudioProcessor::getCurrentProgram() { return 0; }

void IgnitiveAudioProcessor::setCurrentProgram (int index) { }

const juce::String IgnitiveAudioProcessor::getProgramName (int index) { return {}; }

void IgnitiveAudioProcessor::changeProgramName (int index, const juce::String& newName) { }

void IgnitiveAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    juce::dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getTotalNumOutputChannels();

    preFilter.prepare(spec);
    preFilter.reset();

    postFilter.prepare(spec);
	postFilter.reset();

	envelopeFollower.setSampleRate(sampleRate);

    delayLines.clear();
    for (int ch = 0; ch < getTotalNumOutputChannels(); ++ch)
    {
        auto delayLine = std::make_unique<juce::dsp::DelayLine<float>>(2.0 * sampleRate); // 2s buffer
        delayLine->prepare(spec);
        delayLines.push_back(std::move(delayLine));
    }

}

void IgnitiveAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool IgnitiveAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const {
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}
#endif

void IgnitiveAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    int bufferSize = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, bufferSize);

    float pInGain         = apvts.getRawParameterValue("inGain")->load();
    float pOutGain        = apvts.getRawParameterValue("outGain")->load();
    float pMix            = apvts.getRawParameterValue("mix")->load();

    feedbackDest.baseValue      = apvts.getRawParameterValue("feedback")->load();
    feedbackDelayDest.baseValue = apvts.getRawParameterValue("feedbackDelay")->load();
    
    driveDest.baseValue = apvts.getRawParameterValue("drive")->load();
    colorDest.baseValue = apvts.getRawParameterValue("color")->load();
    int pDistortionType = apvts.getRawParameterValue("distortionType")->load();

    preCutoffDest.baseValue = apvts.getRawParameterValue("preCutoff")->load();
    preResonanceDest.baseValue = apvts.getRawParameterValue("preResonance")->load();
    int   pPreFilterType      = apvts.getRawParameterValue("preType")->load();
    bool  pPreFilterOn        = apvts.getRawParameterValue("preFilterOn")->load();

    postCutoffDest.baseValue = apvts.getRawParameterValue("postCutoff")->load();
    postResonanceDest.baseValue = apvts.getRawParameterValue("postResonance")->load();
    int   pPostFilterType      = apvts.getRawParameterValue("postType")->load();
    bool  pPostFilterOn        = apvts.getRawParameterValue("postFilterOn")->load();

	float pEnvAttack = apvts.getRawParameterValue("envAttack")->load();
	float pEnvDecay  = apvts.getRawParameterValue("envDecay")->load();
	float pEnvGate   = apvts.getRawParameterValue("envGate")->load();

	envelopeFollower.setAttackTime(pEnvAttack);
	envelopeFollower.setReleaseTime(pEnvDecay);
	envelopeFollower.setGate(pEnvGate);

    //int delaySamples = (int)(pFeedbackDelay * 0.001 * getSampleRate());

    preFilter.setType(static_cast<juce::dsp::StateVariableTPTFilterType>(pPreFilterType));

    postFilter.setType(static_cast<juce::dsp::StateVariableTPTFilterType>(pPostFilterType));
   
	distortion.setDistortionAlgorithm(static_cast<DistortionType>(pDistortionType));

    buffer.applyGain(pInGain);

    for (int sample = 0; sample < bufferSize; ++sample) {
        float drySample = 0.0f;

		for (int channel = 0; channel < totalNumInputChannels; ++channel)
            drySample += buffer.getWritePointer(channel)[sample];

		drySample /= float(totalNumInputChannels);

		float envValue = envelopeFollower.process(drySample); // Update envelope follower

		modMatrix.update(); // Update modulation matrix
        
        for (int channel = 0; channel < totalNumInputChannels; ++channel) {
			auto* input = buffer.getWritePointer(channel);

            //=============// CLEAN SIGNAL //=============//
            float wetSample = input[sample];
            
            //==============// PRE FILTER //==============//
            if (pPreFilterOn) {
                float preCutoff = 20 * std::pow(10.0f, preCutoffDest.getFinalValue() * 3.0f);

                preFilter.setCutoffFrequency(preCutoff);
                preFilter.setResonance(juce::jmap(preResonanceDest.getFinalValue(), 0.707f, 4.0f));
                wetSample = preFilter.processSample(channel, wetSample);
            }
            
            //==============// DISTORTION //==============//
            distortion.setDrive(driveDest.getFinalValue() * 20.0f);
            wetSample = distortion.processSample(wetSample); // Distort signal

            //==============// FEEDBACK DELAY //==============//
            float delaySamps = feedbackDelayDest.getFinalValue() * getSampleRate();

            auto& delayLine = *delayLines[(size_t)channel];
            delayLine.setDelay(delaySamps);

            float delayedSample = delayLine.popSample(channel);
            wetSample += delayedSample * feedbackDest.getFinalValue();

            //==============// POST FILTER //==============//
            if (pPostFilterOn) {
                float postCutoff = 20 * std::pow(10.0f, postCutoffDest.getFinalValue() * 3.0f);

                postFilter.setCutoffFrequency(postCutoff);
                postFilter.setResonance(juce::jmap(postResonanceDest.getFinalValue(), 0.707f, 4.0f));
                wetSample = postFilter.processSample(channel, wetSample);
            }
            
            //==============// MIX //==============//
            float mixSample = juce::jmap(pMix, input[sample], wetSample);

            input[sample] = mixSample;

            //==============// FEEDBACK WRITE //==============//
            delayLine.pushSample(channel, mixSample);
        }   
    }

    buffer.applyGain(pOutGain);
}

bool IgnitiveAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* IgnitiveAudioProcessor::createEditor() { return new IgnitiveAudioProcessorEditor (*this); }

void IgnitiveAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void IgnitiveAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new IgnitiveAudioProcessor(); }

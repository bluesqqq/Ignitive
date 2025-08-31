#include "PluginProcessor.h"
#include "PluginEditor.h"


juce::AudioProcessorValueTreeState::ParameterLayout IgnitiveAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Gain
    layout.add(std::make_unique<juce::AudioParameterFloat>("inGain", "In Gain", 0.0f, 2.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("outGain", "Out Gain", 0.0f, 2.0f, 1.0f));

    // Distortion + Feedback
    layout.add(std::make_unique<juce::AudioParameterFloat>("drive", "Drive", 0.01f, 20.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("color", "Color", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("distortionType", "Distortion Type", juce::StringArray{ "Hard Clip", "Tube", "Fuzz", "Rectify", "Downsample" }, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("feedback", "Feedback", 0.0f, 0.8f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("feedbackDelay", "Feedback Delay", 1.0f, 100.0f, 50.0f));

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
    layout.add(std::make_unique<juce::AudioParameterFloat>("envAttack", "Attack", 0.0f, 100.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("envDecay", "Decay", 0.0f, 200.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("envGate", "Gate", 0.0f, 1.0f, 0.0f));

    // LFO
	layout.add(std::make_unique<juce::AudioParameterFloat>("lfoRate", "LFO Rate", 0.0f, 20.0f, 0.0f));
	layout.add(std::make_unique<juce::AudioParameterChoice>("lfoWaveform", "LFO Waveform", juce::StringArray{ "Sine", "Triangle", "Square", "Saw" }, 0));

    return layout;
}

IgnitiveAudioProcessor::IgnitiveAudioProcessor()
    : AudioProcessor (BusesProperties().withInput("Input",  juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      distortion(DistortionEngine()) { }

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
	int feedbackBufferSize = sampleRate * 0.1; // 100 ms buffer

	feedbackBuffer.setSize(getTotalNumOutputChannels(), feedbackBufferSize);
    feedbackBuffer.clear();

    feedbackWritePositions.resize(getTotalNumOutputChannels(), 0);


    juce::dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getTotalNumOutputChannels();

    preFilter.prepare(spec);
    preFilter.reset();

    postFilter.prepare(spec);
	postFilter.reset();

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
    int feedbackBufferSize = feedbackBuffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, bufferSize);

    float pInGain         = apvts.getRawParameterValue("inGain")->load();
    float pOutGain        = apvts.getRawParameterValue("outGain")->load();
    float pMix            = apvts.getRawParameterValue("mix")->load();
    float pFeedback       = apvts.getRawParameterValue("feedback")->load();
    float pFeedbackDelay  = apvts.getRawParameterValue("feedbackDelay")->load();
    float pDrive          = apvts.getRawParameterValue("drive")->load();
    int   pDistortionType = apvts.getRawParameterValue("distortionType")->load();

    float pPreFilterCutoff    = apvts.getRawParameterValue("preCutoff")->load();
    float pPreFilterResonance = apvts.getRawParameterValue("preResonance")->load();
    int   pPreFilterType      = apvts.getRawParameterValue("preType")->load();
    bool  pPreFilterOn        = apvts.getRawParameterValue("preFilterOn")->load();

    float pPostFilterCutoff    = apvts.getRawParameterValue("postCutoff")->load();
    float pPostFilterResonance = apvts.getRawParameterValue("postResonance")->load();
    int   pPostFilterType      = apvts.getRawParameterValue("postType")->load();
    bool  pPostFilterOn        = apvts.getRawParameterValue("postFilterOn")->load();

    int delaySamples = (int)(pFeedbackDelay * 0.001 * getSampleRate());

    buffer.applyGain(pInGain);

    float preCutoff = 20 * std::pow(10.0f, pPreFilterCutoff * 3.0f);
    preFilter.setType(static_cast<juce::dsp::StateVariableTPTFilterType>(pPreFilterType));
    postFilter.setType(static_cast<juce::dsp::StateVariableTPTFilterType>(pPostFilterType));
    preFilter.setCutoffFrequency(preCutoff);
    preFilter.setResonance(juce::jmap(pPreFilterResonance, 0.707f, 4.0f));

	distortion.setDistortionAlgorithm(pDistortionType);
	distortion.setDrive(pDrive);

    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto* input = buffer.getWritePointer(channel);
		auto* fb = feedbackBuffer.getWritePointer(channel);

        int& writePos = feedbackWritePositions[channel]; // reference to this channel's write position

        for (int sample = 0; sample < bufferSize; ++sample) {
            //=============// CLEAN SIGNAL //=============//
			float drySample = input[sample];

			float wetSample = drySample;

            //==============// PRE FILTER //==============//
            if (pPreFilterOn) {

				wetSample = preFilter.processSample(channel, wetSample);
            }

            //==============// FEEDBACK //==============//
            int readPos = writePos - delaySamples;
            if (readPos < 0) readPos += feedbackBufferSize;
            float delayedSample = fb[readPos];

			wetSample += delayedSample * pFeedback;

            //==============// DISTORTION //==============//
            distortion.setModulation(0); // Set the modulation

            wetSample = distortion.processSample(wetSample); // Distort signal

            //==============// POST FILTER //==============//
            if (pPostFilterOn) {
                postFilter.setCutoffFrequency(pPostFilterCutoff * 20000.0f);
                postFilter.setResonance(juce::jmap(pPostFilterResonance, 0.707f, 4.0f));

                wetSample = postFilter.processSample(channel, wetSample);
            }

            //==============// MIX //==============//
            float mixSignal = juce::jmap(pMix, drySample, wetSample);

			input[sample] = mixSignal;

            fb[writePos] = input[sample];
            writePos = (writePos + 1) % feedbackBufferSize;
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

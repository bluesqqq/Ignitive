#include "PluginProcessor.h"
#include "PluginEditor.h"


juce::AudioProcessorValueTreeState::ParameterLayout IgnitiveAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("inGain", "In Gain", 0.0f, 2.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("outGain", "Out Gain", 0.0f, 2.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("feedback", "Feedback", 0.0f, 0.8f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("feedbackDelay", "Feedback Delay", 1.0f, 100.0f, 50.0f));

    return layout;
}

IgnitiveAudioProcessor::IgnitiveAudioProcessor()
    : AudioProcessor (BusesProperties().withInput("Input",  juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)) { }

IgnitiveAudioProcessor::~IgnitiveAudioProcessor() { }

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

    float pInGain = apvts.getRawParameterValue("inGain")->load();
    float pOutGain = apvts.getRawParameterValue("outGain")->load();
    float pFeedback = apvts.getRawParameterValue("feedback")->load();
    float pFeedbackDelay = apvts.getRawParameterValue("feedbackDelay")->load();

    int delaySamples = (int)(pFeedbackDelay * 0.001 * getSampleRate());

    buffer.applyGain(pInGain);

    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto* input = buffer.getWritePointer(channel);
		auto* fb = feedbackBuffer.getWritePointer(channel);

        int& writePos = feedbackWritePositions[channel]; // reference to this channel's write position

        for (int sample = 0; sample < bufferSize; ++sample) {
            int readPos = writePos - delaySamples;
            if (readPos < 0) readPos += feedbackBufferSize;

            float delayedSample = fb[readPos];
            input[sample] += delayedSample * pFeedback;

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

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"

IgnitiveAudioProcessor::IgnitiveAudioProcessor()
    : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "Parameter", Parameters::createParameterLayout()),
      ignitive(parameters, *this) {

    loadAllPresets();
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

    ignitive.prepare(spec);
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

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    ignitive.process(context);
}

bool IgnitiveAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* IgnitiveAudioProcessor::createEditor() { return new IgnitiveAudioProcessorEditor (*this); }

void IgnitiveAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    auto state = parameters.copyState();

    ignitive.modMatrix.saveModConnectionsToState(state);

    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    copyXmlToBinary(*xml, destData);
}

void IgnitiveAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType())) {
        juce::ValueTree tree = juce::ValueTree::fromXml(*xmlState);

        parameters.replaceState(tree.createCopy());

        ignitive.modMatrix.loadModConnectionsFromState(tree);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new IgnitiveAudioProcessor(); }
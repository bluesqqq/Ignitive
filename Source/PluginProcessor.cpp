#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"

juce::ValueTree IgnitiveAudioProcessor::getState() {
    auto state = parameters.copyState();
    ignitive.modMatrix.saveModConnectionsToState(state);
    state.setProperty("PresetName", currentPresetName, nullptr);
    return state;
}



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
    auto state = getState();

    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void IgnitiveAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr && xmlState->hasTagName(parameters.state.getType())) {
        juce::ValueTree tree = juce::ValueTree::fromXml(*xmlState);
        parameters.replaceState(tree);
        ignitive.modMatrix.loadModConnectionsFromState(tree);
        currentPresetName = tree.getProperty("PresetName", "Init");
    }
}

bool IgnitiveAudioProcessor::loadPreset(Preset* preset) {
    if (preset == nullptr) return false;

    auto state = preset->getState();
    if (!state.isValid()) return false;

    if (!ignitive.modMatrix.loadModConnectionsFromState(state)) return false;
    parameters.replaceState(state);

    currentPresetName = preset->getName();

    return true;
}

bool IgnitiveAudioProcessor::loadPreset(int index) {
    if (index < 0 || index >= presets.size()) return false;

    auto& preset = presets[index];
    return loadPreset(preset.get());
}

void IgnitiveAudioProcessor::savePreset() {
    auto dir = getUserPresetFolder();

    auto chooser = std::make_shared<juce::FileChooser>("Save Preset", dir, "*.xml");

    chooser->launchAsync(juce::FileBrowserComponent::saveMode |
        juce::FileBrowserComponent::canSelectFiles,
        [this, chooser](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.exists() || file.getParentDirectory().exists()) {
				currentPresetName = file.getFileNameWithoutExtension();
                auto state = getState();

                if (auto xml = state.createXml())
                    xml->writeTo(file);
            }
        });
}

juce::File IgnitiveAudioProcessor::getUserPresetFolder() const {
    auto baseDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);

    juce::File dir = baseDir.getChildFile("Ignitive").getChildFile("Presets");

    if (!dir.exists())
        dir.createDirectory();

    return dir;
}

void IgnitiveAudioProcessor::loadAllPresets() {
    presets.clear();

    // Init Preset
    presets.push_back(std::make_unique<FactoryPreset>("Init", BinaryData::Init_xml, BinaryData::Init_xmlSize));

    // Factory Presets
    for (auto& res : factoryPresets) {
        presets.push_back(std::make_unique<FactoryPreset>(res.name, res.data, res.size));
    }

    // User Presets
    auto dir = getUserPresetFolder();
    auto files = dir.findChildFiles(juce::File::findFiles, false, "*.xml");

    for (auto& file : files) {
        auto name = file.getFileNameWithoutExtension();
        presets.push_back(std::make_unique<UserPreset>(name, file));
    }
}

void IgnitiveAudioProcessor::randomize() {
    for (auto& id : Parameters::getRandomizeParameters()) {
        auto* p = parameters.getParameter(id);
        if (p != nullptr) {
            float randomValue = juce::Random::getSystemRandom().nextFloat();
            p->setValueNotifyingHost(randomValue);
        }
    }

	parameters.getParameter(Parameters::ID_DISTORTION_TYPE)->setValueNotifyingHost(juce::Random::getSystemRandom().nextFloat());
    parameters.getParameter(Parameters::ID_CHARACTER_TYPE)->setValueNotifyingHost(juce::Random::getSystemRandom().nextFloat());
    parameters.getParameter(Parameters::ID_CHARACTER_POLARITY)->setValueNotifyingHost(juce::Random::getSystemRandom().nextFloat());

    ignitive.modMatrix.randomizeConnections();

    parameters.state = parameters.copyState();
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new IgnitiveAudioProcessor(); }
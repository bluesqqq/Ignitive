#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>

#include "IgnitiveEngine.h"

class Preset {
    private:
        juce::String name;

    protected:
        virtual std::unique_ptr<juce::XmlElement> getXml() = 0;

    public:
        Preset(const juce::String& name) : name(name) { }

        juce::String getName() const { return name; }

        juce::ValueTree getState() {
            auto xml = getXml();
            if (xml == nullptr) return {};

            auto state = juce::ValueTree::fromXml(*xml);
            return state.isValid() ? state : juce::ValueTree{};
        }
};

class UserPreset : public Preset {
    private:
        juce::File file;

    protected:
        std::unique_ptr<juce::XmlElement> getXml() override {
            if (!file.exists()) return {};
            return juce::XmlDocument::parse(file);
        }

    public:
        UserPreset(const juce::String& name, const juce::File& file) : Preset(name), file(file) {}
};

class FactoryPreset : public Preset {
    private:
        const char* binaryData = nullptr;
        int binarySize = 0;

    protected:
        std::unique_ptr<juce::XmlElement> getXml() override {
            juce::String xmlString = juce::String::fromUTF8(binaryData, binarySize);
            return juce::parseXML(xmlString);
        }

    public:
        FactoryPreset(const juce::String& name, const char* binaryData, int binarySize) : Preset(name), binaryData(binaryData), binarySize(binarySize) {}
};

class IgnitiveAudioProcessor : public juce::AudioProcessor {
    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IgnitiveAudioProcessor)

    public:
        juce::AudioProcessorValueTreeState parameters;

        IgnitiveEngine ignitive;

        std::vector<std::unique_ptr<Preset>> presets;

        IgnitiveAudioProcessor();
        ~IgnitiveAudioProcessor() override;

        void prepareToPlay (double sampleRate, int samplesPerBlock) override;
        void releaseResources() override;

        bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

        void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

        juce::AudioProcessorEditor* createEditor() override;
        bool hasEditor() const override;

        const juce::String getName() const override;

        bool acceptsMidi() const override;
        bool producesMidi() const override;
        bool isMidiEffect() const override;
        double getTailLengthSeconds() const override;

        int getNumPrograms() override;
        int getCurrentProgram() override;
        void setCurrentProgram (int index) override;
        const juce::String getProgramName (int index) override;
        void changeProgramName (int index, const juce::String& newName) override;

        void getStateInformation (juce::MemoryBlock& destData) override;
        void setStateInformation (const void* data, int sizeInBytes) override;
        
        // PRESETS
        bool loadPreset(Preset* preset) {
            if (preset == nullptr) return false;

            auto state = preset->getState();
            if (!state.isValid()) return false;

            if (!ignitive.modMatrix.loadModConnectionsFromState(state)) return false;
            parameters.replaceState(state);

            return true;
        }

        bool loadPreset(int index) {
            if (index < 0 || index >= presets.size()) return false;

            auto& preset = presets[index];
            return loadPreset(preset.get());
        }

        void savePreset() {
            auto dir = getUserPresetFolder();

            auto chooser = std::make_shared<juce::FileChooser>("Save Preset", dir, "*.xml");

            chooser->launchAsync(juce::FileBrowserComponent::saveMode |
                juce::FileBrowserComponent::canSelectFiles,
                [this, chooser](const juce::FileChooser& fc) {
                    auto file = fc.getResult();
                    if (file.exists() || file.getParentDirectory().exists()) {
                        auto state = parameters.copyState();
                        ignitive.modMatrix.saveModConnectionsToState(state);

                        if (auto xml = state.createXml())
                            xml->writeTo(file);
                    }
                });
        }

        juce::File getUserPresetFolder() const {
            auto baseDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);

            juce::File dir = baseDir.getChildFile("Ignitive").getChildFile("Presets");

            if (!dir.exists())
                dir.createDirectory();

            return dir;
        }

        void loadAllPresets() {
            presets.clear();

            // Factory Presets

            // User Presets
            auto dir = getUserPresetFolder();
            auto files = dir.findChildFiles(juce::File::findFiles, false, "*.xml");

            for (auto& file : files) {
                auto name = file.getFileNameWithoutExtension();
                presets.push_back(std::make_unique<UserPreset>(name, file));
            }
        }

        void randomize() {
            // This works fine for now but it could be improved especially for distortion and character type selections.

            for (auto& id : Parameters::modulatableParameters) {
                auto* p = parameters.getParameter(id);
                if (p != nullptr) {
                    float randomValue = juce::Random::getSystemRandom().nextFloat();
                    p->setValueNotifyingHost(randomValue);
                }
            }

            ignitive.modMatrix.randomizeConnections();

            parameters.state = parameters.copyState();
        }
};

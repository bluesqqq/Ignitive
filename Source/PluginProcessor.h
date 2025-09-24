#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>

#include "IgnitiveEngine.h"
#include "Presets.h"

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
        bool loadPreset(Preset* preset);
        bool loadPreset(int index);

        // Saves the current patch as a preset
        void savePreset();

        juce::File getUserPresetFolder() const;

        void loadAllPresets();

        void randomize();
};

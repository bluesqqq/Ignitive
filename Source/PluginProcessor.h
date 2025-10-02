#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>

#include "IgnitiveEngine.h"
#include "Presets.h"

class IgnitiveAudioProcessor : public juce::AudioProcessor {
    private:
        juce::ValueTree getState();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IgnitiveAudioProcessor)

    public:
        juce::AudioProcessorValueTreeState parameters;

        IgnitiveEngine ignitive;

        std::vector<std::unique_ptr<Preset>> presets;
        juce::String currentPresetName;

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
        
		// ============/ Presets /============ //

        bool loadPreset(Preset* preset);
        bool loadPreset(int index);

        /// <summary>
		/// Saves the current state as a new preset in the user presets folder.
        /// </summary>
        void savePreset();

        juce::File getUserPresetFolder() const;

        /// <summary>
        /// Loads all available presets.
        /// </summary>
        void loadAllPresets();

        /// <summary>
		/// Generates random values for all randomizable parameters + random modulation connections.
        /// </summary>
        void randomize();
};

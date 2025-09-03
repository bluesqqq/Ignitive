#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>

#include "DistortionEngine.h"
#include "EnvelopeFollower.h"
#include "ModSource.h"

class IgnitiveAudioProcessor : public juce::AudioProcessor {
    private:
        juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

        //==============// DSP //==============//
        DistortionEngine distortion;

        std::vector<std::unique_ptr<juce::dsp::DelayLine<float>>> delayLines;

        juce::dsp::StateVariableTPTFilter<float> preFilter, postFilter;

        //==============// MODULATION //==============//
        
        // Souces
        EnvelopeFollower envelopeFollower;

		// Destinations
        ModDestination driveDest{ "DRIVE" };
        ModDestination colorDest{ "COLOR" };
        ModDestination preCutoffDest{ "PRECUT" };
        ModDestination preResonanceDest{ "PRERES" };
        ModDestination postCutoffDest{ "POSTCUT" };
        ModDestination postResonanceDest{ "POSTRES" };
        ModDestination feedbackDest{ "FEEDBK" };
        ModDestination feedbackDelayDest{ "FDBK DL" };



        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IgnitiveAudioProcessor)

    public:
        ModMatrix modMatrix;

        juce::AudioProcessorValueTreeState apvts{*this, nullptr, "Parameters", createParameterLayout()};

        IgnitiveAudioProcessor();
        ~IgnitiveAudioProcessor() override;

        void prepareToPlay (double sampleRate, int samplesPerBlock) override;
        void releaseResources() override;

       #ifndef JucePlugin_PreferredChannelConfigurations
        bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
       #endif

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

        float getEnvelopeValue() { return envelopeFollower.getEnvelope(); }

		DistortionEngine& getDistortionEngine() { return distortion; }

        std::vector<ModDestination*> getDestinations() {
            return {
                &driveDest,
                &colorDest,
                &preCutoffDest,
                &preResonanceDest,
                &postCutoffDest,
                &postResonanceDest,
                &feedbackDest,
                &feedbackDelayDest
            };
        }

        std::vector<ModSource*> getSources() {
            return {
				&envelopeFollower
            };
        }
};

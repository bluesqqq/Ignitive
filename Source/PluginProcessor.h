#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>

#include "IgnitiveEngine.h"

#include "EnvelopeFollower.h"
#include "ModMatrix.h"

class IgnitiveAudioProcessor : public juce::AudioProcessor {
    private:
        //==============// MODULATION //==============//
        
        // Souces
        EnvelopeFollower envelopeFollower{ "ENV", "envelope" };

		// Destinations
        ModDestination driveDest        { "DRIVE",   "drive" };
        ModDestination colorDest        { "COLOR",   "color" };
        ModDestination preCutoffDest    { "PRECUT",  "preFilterCutoff" };
        ModDestination preResonanceDest { "PRERES",  "preFilterResonance" };
        ModDestination postCutoffDest   { "POSTCUT", "postFilterCutoff" };
        ModDestination postResonanceDest{ "POSTRES", "postFilterResonance" };
        ModDestination feedbackDest     { "FEEDBK",  "feedbackAmount" , 0.0f, 0.8f };
        ModDestination feedbackDelayDest{ "FDBK DL",  "feedbackDelay" , 0.001f, 0.1f};

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IgnitiveAudioProcessor)

    public:
        IgnitiveEngine ignitive;

        juce::AudioProcessorValueTreeState parameters;

        ModMatrix modMatrix;

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

        float getEnvelopeValue() { return envelopeFollower.getEnvelope(); }
};

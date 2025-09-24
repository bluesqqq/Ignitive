#pragma once

#include <JuceHeader.h>

class Preset {
    private:
        juce::String name;

    protected:
        virtual std::unique_ptr<juce::XmlElement> getXml() = 0;

    public:
        Preset(const juce::String& name);

        juce::String getName() const;

        juce::ValueTree getState();
};

class UserPreset : public Preset {
    private:
        juce::File file;

    protected:
        std::unique_ptr<juce::XmlElement> getXml() override;

    public:
        UserPreset(const juce::String& name, const juce::File& file);
};

class FactoryPreset : public Preset {
    private:
        const char* binaryData = nullptr;
        int binarySize = 0;

    protected:
        std::unique_ptr<juce::XmlElement> getXml() override;

    public:
        FactoryPreset(const juce::String& name, const char* binaryData, int binarySize);
};
#pragma once

#include <JuceHeader.h>

struct PresetResource {
    const char* name;
    const char* data;
    int size;
};

static const PresetResource factoryPresets[] =
{
    { "Bit Battle",      BinaryData::Bit_Battle_xml,      BinaryData::Bit_Battle_xmlSize },
    { "Chirp Machine",   BinaryData::Chirp_Machine_xml,   BinaryData::Chirp_Machine_xmlSize },
    { "Fizzing Amp",     BinaryData::Fizzing_Amp_xml,     BinaryData::Fizzing_Amp_xmlSize },
    { "Hot Air",         BinaryData::Hot_Air_xml,         BinaryData::Hot_Air_xmlSize },
    { "Hydrophone",      BinaryData::Hydrophone_xml,      BinaryData::Hydrophone_xmlSize },
    { "Punchy",          BinaryData::Punchy_xml,          BinaryData::Punchy_xmlSize },
    { "Rectifying",      BinaryData::Rectifying_xml,      BinaryData::Rectifying_xmlSize },
    { "Shiver",          BinaryData::Shiver_xml,          BinaryData::Shiver_xmlSize },
    { "Snarl",           BinaryData::Snarl_xml,           BinaryData::Snarl_xmlSize },
    { "Squeegee",        BinaryData::Squeegee_xml,        BinaryData::Squeegee_xmlSize },
    { "Static Electricity", BinaryData::Static_Electricity_xml, BinaryData::Static_Electricity_xmlSize },
    { "Tapeworm",        BinaryData::Tapeworm_xml,        BinaryData::Tapeworm_xmlSize },
    { "Tightener",       BinaryData::Tightener_xml,       BinaryData::Tightener_xmlSize },
    { "Unphased",        BinaryData::Unphased_xml,        BinaryData::Unphased_xmlSize },
};

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
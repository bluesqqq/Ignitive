#include "Presets.h"

Preset::Preset(const juce::String& name) : name(name) {}

juce::String Preset::getName() const { return name; }

juce::ValueTree Preset::getState() {
    auto xml = getXml();
    if (xml == nullptr) return {};

    auto state = juce::ValueTree::fromXml(*xml);
    return state.isValid() ? state : juce::ValueTree{};
}



std::unique_ptr<juce::XmlElement> UserPreset::getXml() {
    if (!file.exists()) return {};
    return juce::XmlDocument::parse(file);
}

UserPreset::UserPreset(const juce::String& name, const juce::File& file) : Preset(name), file(file) {}



std::unique_ptr<juce::XmlElement> FactoryPreset::getXml() {
    juce::String xmlString = juce::String::fromUTF8(binaryData, binarySize);
    return juce::parseXML(xmlString);
}

FactoryPreset::FactoryPreset(const juce::String& name, const char* binaryData, int binarySize) : Preset(name), binaryData(binaryData), binarySize(binarySize) {}
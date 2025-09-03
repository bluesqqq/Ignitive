#include "ModSource.h"

ModSource::ModSource(const juce::String& n, const juce::String& srcID) : name(n), sourceID(srcID) {}



ModDestination::ModDestination(const juce::String& n, const juce::String& dstID, float mn, float mx) : name(n), destinationID(dstID), minValue(mn), maxValue(mx) {}

float ModDestination::getFinalValue() const { return std::clamp(baseValue + modAmount * getRange(), minValue, maxValue); }

float ModDestination::getRange() const { return maxValue - minValue; }



ModConnection::ModConnection(ModSource* src, ModDestination* dst, float d) : source(src), destination(dst), depth(d) {}
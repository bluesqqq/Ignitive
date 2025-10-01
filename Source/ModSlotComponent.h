#pragma once

#include <JuceHeader.h>
#include "CustomLAFs.h"
#include "ModMatrix.h"

class ModSlotComponent : public juce::Component {
private:
	ModSlotLAF modSlotLAF; // TODO: make this a reference to save some memorys

	juce::ComboBox destinationBox;
	juce::Slider depthSlider;

	ModConnection* connection = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModSlotComponent)

public:
	ModSlotComponent(ModConnection* conn, ModMatrix& modMatrix);

	void paint(juce::Graphics& g) override;

	void setDestinations(ModMatrix& modMatrix);

	void resized() override;
};
#pragma once

#include <JuceHeader.h>
#include "ModSource.h"
#include "PluginProcessor.h"
#include "CustomLAFs.h"

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

		void resized() override;
};

class ModMatrixComponent : public juce::Component {
	private:
		juce::OwnedArray<ModSlotComponent> modSlots;

		ModMatrix& modMatrix;

		// Only shows connections from this source
		juce::String sourceIDFilter;

		void rebuildSlots();

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModMatrixComponent)

	public:
		ModMatrixComponent(ModMatrix& matrix);

		void setSourceIDFilter(const juce::String& sourceID);

		void resized() override;
};
#pragma once

#include <JuceHeader.h>
#include "ModSlotComponent.h"
#include "ModMatrix.h"

class ModMatrixComponent : public juce::Component {
	private:
		juce::OwnedArray<ModSlotComponent> modSlots;

		ModMatrix& modMatrix;

		// Only shows connections from this source
		juce::String sourceIDFilter;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModMatrixComponent)

	public:
		ModMatrixComponent(ModMatrix& matrix);

		void rebuildSlots();
		void setSourceIDFilter(const juce::String& sourceID);

		void resized() override;
};
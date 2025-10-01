#include "ModMatrixComponent.h"

void ModMatrixComponent::rebuildSlots() {
	modSlots.clear(true);

	auto& connections = modMatrix.getConnections();

	for (auto& connection : connections) {
		if (sourceIDFilter.isEmpty() || connection.sourceID == sourceIDFilter) {
			auto* slot = new ModSlotComponent(&connection, modMatrix);
			modSlots.add(slot);
			addAndMakeVisible(slot);
		}
	}

	resized();
}

ModMatrixComponent::ModMatrixComponent(ModMatrix& matrix) : modMatrix(matrix), sourceIDFilter("") {
	rebuildSlots();
}

void ModMatrixComponent::setSourceIDFilter(const juce::String& sourceID) {
	sourceIDFilter = sourceID;
	rebuildSlots();
}

void ModMatrixComponent::resized() {
	if (modSlots.size() == 0) return;

	auto area = getLocalBounds().toFloat();
	float slotHeight = area.getHeight() / (float)modSlots.size();

	for (int i = 0; i < modSlots.size(); ++i) {
		auto slotArea = area.removeFromTop(slotHeight);
		modSlots[i]->setBounds(slotArea.toNearestInt());
	}
}

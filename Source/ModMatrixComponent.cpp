#include "ModMatrixComponent.h"

ModSlotComponent::ModSlotComponent(ModConnection* conn, ModMatrix& modMatrix) : connection(conn) {
	if (connection == nullptr) return;

	// Destination selector
	int selectedID = 1;
	int id = 1;

	destinationBox.addItem("---", id++);
	auto& destinations = modMatrix.getDestinationDisplayNameAndIDs();
	for (auto& destination : destinations) {
		destinationBox.addItem(destination.second, id);
		if (connection && destination.first == connection->destinationID) selectedID = id;
		++id;
	}

	destinationBox.setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
	destinationBox.setLookAndFeel(&modSlotLAF);
	destinationBox.setSelectedId(selectedID, juce::dontSendNotification);
	destinationBox.onChange = [this, destinations]() {
		if (connection) {
			int selectedID = destinationBox.getSelectedId();

			if (selectedID == 1) { // No Destination
				connection->destinationID = "";
				depthSlider.setValue(0, juce::NotificationType::dontSendNotification);
				depthSlider.setEnabled(false);
			}
			else {
				if (connection->destinationID.isEmpty()) { // If coming from no destination, set the depth
					depthSlider.setValue(connection->depth);
				}
				connection->destinationID = destinations[selectedID - 2].first;
				depthSlider.setEnabled(true);
			}
		}
	};
	addAndMakeVisible(destinationBox);

	// Depth slider
	depthSlider.setLookAndFeel(&modSlotLAF);
	depthSlider.setRange(-1.0f, 1.0f, 0.01f);
	addAndMakeVisible(depthSlider);
	if (connection) depthSlider.setValue(connection->depth);
	depthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	depthSlider.onValueChange = [this]() {
		if (connection) connection->depth = (float)depthSlider.getValue();
	};

	if (connection->destinationID.isEmpty()) depthSlider.setEnabled(false);
}

void ModSlotComponent::paint(juce::Graphics& g) {
	auto bounds = getLocalBounds();

	const int pixelSize = 5;
	int wPixels = bounds.getWidth() / pixelSize;
	float pixelY = bounds.getY() + bounds.getHeight() - pixelSize;

	g.setColour(juce::Colours::yellow);

	for (int ix = 0; ix < wPixels; ix++) {
		juce::Rectangle<float> pixel(bounds.getX() + ix * pixelSize, pixelY, pixelSize - 1.0f, pixelSize - 1.0f);

		g.fillRect(pixel);
	}
}

void ModSlotComponent::resized() {
	auto bounds = getLocalBounds();

	bounds = bounds.withTrimmedBottom(5);

	auto leftHalf = bounds.removeFromLeft(bounds.getWidth() / 2);
	destinationBox.setBounds(leftHalf);

	depthSlider.setBounds(bounds);
}



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

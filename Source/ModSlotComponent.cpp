#include "ModSlotComponent.h"

ModSlotComponent::ModSlotComponent(ModConnection* conn, ModMatrix& modMatrix) : connection(conn) {
	if (connection == nullptr) return;

	// Destination selector
	setDestinations(modMatrix);
	destinationBox.setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
	destinationBox.setLookAndFeel(&modSlotLAF);
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

void ModSlotComponent::setDestinations(ModMatrix& modMatrix) {
	// Destination selector
	int selectedID = 1;
	int id = 1;

	destinationBox.addItem("---", id++); // No destination
	auto& destinations = modMatrix.getDestinationDisplayNameAndIDs();
	for (auto& destination : destinations) {
		destinationBox.addItem(destination.second, id);
		if (connection && destination.first == connection->destinationID) selectedID = id;
		++id;
	}

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
}

void ModSlotComponent::resized() {
	auto bounds = getLocalBounds();

	bounds = bounds.withTrimmedBottom(5);

	auto leftHalf = bounds.removeFromLeft(bounds.getWidth() / 2);
	destinationBox.setBounds(leftHalf);

	depthSlider.setBounds(bounds);
}


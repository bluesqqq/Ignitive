#pragma once

#include <JuceHeader.h>
#include "ModSource.h"
#include "PluginProcessor.h"
#include "CustomLAFs.h"

class ModSlotComponent : public juce::Component {
private:
	ModSlotLAF modSlotLAF;

	juce::ComboBox destinationBox;
	juce::Slider depthSlider;

	ModConnection* connection = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModSlotComponent)

public:
	ModSlotComponent(ModConnection* conn, ModMatrix& modMatrix) : connection(conn) {
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

				if (selectedID == 1) {
					connection->destinationID = ""; // No Destination
					connection->depth = 0.0f;
				}
				else {
					if (connection->destinationID.isEmpty()) connection->depth = 0.5f; // If coming from no destination, set the depth
					connection->destinationID = destinations[selectedID - 2].first;
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
	}

	void paint(juce::Graphics& g) override {
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

	void resized() override {
		auto bounds = getLocalBounds();

		bounds = bounds.withTrimmedBottom(5);

		// Split remaining 90% in half
		auto leftHalf = bounds.removeFromLeft(bounds.getWidth() / 2);
		destinationBox.setBounds(leftHalf);

		depthSlider.setBounds(bounds); // remaining right half
	}
};

class ModMatrixComponent : public juce::Component {
	private:
		juce::OwnedArray<ModSlotComponent> modSlots;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModMatrixComponent)

			ModMatrix& modMatrix;

		juce::String sourceIDFilter;

		void rebuildSlots() {
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

	public:
		ModMatrixComponent(ModMatrix& matrix) : modMatrix(matrix), sourceIDFilter("") {
			rebuildSlots();
		}

		void setSourceIDFilter(const juce::String& sourceID) {
			sourceIDFilter = sourceID;
			rebuildSlots();
		}

		void resized() override {
			if (modSlots.size() == 0) return;

			auto area = getLocalBounds().toFloat();
			float slotHeight = area.getHeight() / (float)modSlots.size();

			for (int i = 0; i < modSlots.size(); ++i) {
				auto slotArea = area.removeFromTop(slotHeight);
				modSlots[i]->setBounds(slotArea.toNearestInt());
			}
		}
};
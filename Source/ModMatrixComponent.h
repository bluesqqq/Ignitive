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
		juce::TextButton removeButton;

		ModConnection* connection = nullptr;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModSlotComponent)

	public:
		ModSlotComponent(ModConnection* conn, ModMatrix& modMatrix) : connection(conn) {
			if (connection == nullptr) return;

			// Destination selector
			int selectedID = 0;
			int id = 1;

			auto& destinations = modMatrix.getDestinationDisplayNameAndIDs();
			for (auto& destination : destinations) {
				destinationBox.addItem(destination.second, id);
				if (connection && destination.first == connection->destinationID) selectedID = id;
				++id;
			}

			destinationBox.setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
			destinationBox.setLookAndFeel(&modSlotLAF);
			if (selectedID > 0) destinationBox.setSelectedId(selectedID, juce::dontSendNotification);
			destinationBox.onChange = [this, destinations]() {
				if (connection) connection->destinationID = destinations[destinationBox.getSelectedId() - 1].first;
			};
			addAndMakeVisible(destinationBox);

			depthSlider.setLookAndFeel(&modSlotLAF);
			depthSlider.setRange(-1.0f, 1.0f, 0.01f);
			addAndMakeVisible(depthSlider);
			if (connection) depthSlider.setValue(connection->depth);
			depthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
			depthSlider.onValueChange = [this]() {
				if (connection) connection->depth = (float)depthSlider.getValue();
			};

			removeButton.setLookAndFeel(&modSlotLAF);
			addAndMakeVisible(removeButton);
			removeButton.setButtonText("X");
			removeButton.onClick = [this]() {
				if (onRemove) onRemove();
			};
		}

		void paint(juce::Graphics& g) override {
			auto area = getLocalBounds();

			g.setColour(juce::Colours::yellow);
			g.drawLine(0, area.getHeight() - 1, area.getWidth(), area.getHeight() - 1);
		}

		void resized() override {
			auto area = getLocalBounds();

			// 10% width for removeButton
			int removeWidth = area.getWidth() * 0.1f;
			auto removeArea = area.removeFromRight(removeWidth);
			removeButton.setBounds(removeArea);

			// Split remaining 90% in half
			auto leftHalf = area.removeFromLeft(area.getWidth() / 2);
			destinationBox.setBounds(leftHalf);

			depthSlider.setBounds(area); // remaining right half
		}

		std::function<void()> onRemove;
};

class ModMatrixComponent : public juce::Component {
	private:
		juce::OwnedArray<ModSlotComponent> modSlots;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModMatrixComponent)

		ModMatrix& modMatrix;

	public:
		ModMatrixComponent(ModMatrix& matrix) : modMatrix(matrix) {
			auto& connections = modMatrix.getConnections();
			
			for (auto& connection : connections) {
				addSlot(connection);
			}
		}

		void addSlot(ModConnection& connection) {
			auto* slot = new ModSlotComponent(&connection, modMatrix);

			const juce::String sourceID = connection.sourceID;
			const juce::String destinationID = connection.destinationID;

			// Not a fan of this, probably a better way
			slot->onRemove = [this, slot, sourceID, destinationID]() {
				modMatrix.removeConnection(sourceID, destinationID);
				modSlots.removeObject(slot, false);
				resized();
				delete slot;
			};

			modSlots.add(slot);
			addAndMakeVisible(slot);
			resized();
		}

		void resized() override {
			auto area = getLocalBounds().reduced(10);

			int slotHeight = 32;
			int y = 0;
			for (auto* slot : modSlots) {
				slot->setBounds(area.removeFromTop(slotHeight));
			}
		}
};
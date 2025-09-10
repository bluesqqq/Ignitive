#pragma once

#include <JuceHeader.h>
#include "ModSource.h"
#include "PluginProcessor.h"

class ModSlotComponent : public juce::Component {
	private:
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

			auto& destinations = modMatrix.getDestinationsIDs();
			for (auto& destination : destinations) {
				destinationBox.addItem(destination, id);
				if (connection && destination == connection->destinationID) selectedID = id;
				++id;
			}

			addAndMakeVisible(destinationBox);

			if (selectedID > 0) destinationBox.setSelectedId(selectedID, juce::dontSendNotification);

			destinationBox.onChange = [this, destinations]() {
				if (connection) connection->destinationID = destinations[destinationBox.getSelectedId() - 1];
			};

			addAndMakeVisible(depthSlider);
			depthSlider.setRange(-1.0f, 1.0f, 0.01f);
			if (connection) depthSlider.setValue(connection->depth);
			depthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
			depthSlider.onValueChange = [this]() {
				if (connection) connection->depth = (float)depthSlider.getValue();
			};

			//addAndMakeVisible(removeButton);
			removeButton.setButtonText("X");
			removeButton.onClick = [this]() {
				if (onRemove) onRemove();
			};
		}

		void resized() override {
			auto area = getLocalBounds();

			auto left = area.removeFromLeft(area.getWidth() / 2);
			destinationBox.setBounds(left);

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
				auto* slot = new ModSlotComponent(&connection, modMatrix);
				modSlots.add(slot);
				addAndMakeVisible(slot);
			}
		}

		void paint(juce::Graphics& g) override {

		}

		void addSlot(const juce::String& sourceID, const juce::String& destinationID, float depthValue = 0.5f) {
			ModConnection* connection = modMatrix.makeConnection(sourceID, destinationID, depthValue);

			if (connection == nullptr) return;
			
			auto* slot = new ModSlotComponent(connection, modMatrix);

			slot->onRemove = [this, connection, slot]() {
				modMatrix.removeConnection(connection->destinationID, connection->sourceID);
				modSlots.removeObject(slot, true);
				resized();
			};

			modSlots.add(slot);
			addAndMakeVisible(slot);
			resized();
		}

		void resized() override {
			auto area = getLocalBounds().reduced(4);

			int slotHeight = 40;
			int y = 0;
			for (auto* slot : modSlots) {
				slot->setBounds(area.removeFromTop(slotHeight));
			}
		}
};
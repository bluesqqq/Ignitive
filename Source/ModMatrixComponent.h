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
		std::vector<ModDestination*> destinations;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModSlotComponent)

	public:
		ModSlotComponent(ModConnection* conn, std::vector<ModDestination*> dsts) : connection(conn), destinations(dsts) {
			
			// Destination selector
			int id = 1;
			for (auto* d : destinations) destinationBox.addItem(d->name, id++);

			// Set to current destination
			if (connection && connection->destination) {
				for (int i = 0; i < destinations.size(); ++i)
					if (destinations[i] == connection->destination)
						destinationBox.setSelectedId(i + 1, juce::dontSendNotification);
			}
			else {
				destinationBox.setSelectedId(1, juce::dontSendNotification);
			};

			addAndMakeVisible(destinationBox);
			destinationBox.onChange = [this]() {
				if (connection) connection->destination = destinations[destinationBox.getSelectedId() - 1];
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

	public:
		ModMatrixComponent() {
		}

		void paint(juce::Graphics& g) override {
			g.fillAll(juce::Colours::darkgrey.darker());
		}

		void addSlot(IgnitiveAudioProcessor& processor, ModSource* src, ModDestination* dst, float depthValue = 0.5f) {
			auto destinations = processor.getDestinations();
			ModConnection* conn = processor.modMatrix.makeConnection(src, dst, depthValue);
			
			auto* slot = new ModSlotComponent(conn, destinations);

			slot->onRemove = [this, &processor, conn, slot]() {
				processor.modMatrix.removeConnection(conn);
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
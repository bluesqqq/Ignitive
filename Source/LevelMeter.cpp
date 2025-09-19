#include "LevelMeter.h"

LevelMeter::LevelMeter(GainProcessor& gainProcessor) : gainProcessor(gainProcessor) {
	startTimer(1000 / 60);
}

void LevelMeter::paint(juce::Graphics& g) {
	auto bounds = getLocalBounds().toFloat().reduced(5.0f);

	float level = gainProcessor.getPeak();

	float meterHeight = juce::jmap(juce::jmin(1.0f, level), 0.0f, bounds.getHeight());

	juce::Rectangle<float> meter(bounds.getX(), bounds.getY() + bounds.getHeight() - meterHeight, bounds.getWidth(), meterHeight);

	g.setColour(level <= 1.0f ? juce::Colours::green : juce::Colours::red);
	g.fillRect(meter);
}

void LevelMeter::timerCallback() {
	repaint();
}

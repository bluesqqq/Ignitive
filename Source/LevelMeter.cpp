#include "LevelMeter.h"

LevelMeter::LevelMeter(GainProcessor& gainProcessor) : gainProcessor(gainProcessor) {
	startTimer(1000 / 60);
}

void LevelMeter::paint(juce::Graphics& g) {
	auto bounds = getLocalBounds().toFloat().reduced(3.0f);

	float level = gainProcessor.getPeak();

	if (level > 1.0f) {
		g.setColour(juce::Colours::red);
		g.fillEllipse(bounds);
	} else if (level > 0.5f) {
		float alpha = juce::jmap(level, 0.5f, 1.0f, 0.0f, 1.0f);

		g.setColour(juce::Colours::green.withAlpha(alpha));
		g.fillEllipse(bounds);
	}
}

void LevelMeter::timerCallback() {
	repaint();
}

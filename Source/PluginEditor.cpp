#include "PluginProcessor.h"
#include "PluginEditor.h"

IgnitiveAudioProcessorEditor::IgnitiveAudioProcessorEditor (IgnitiveAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p) {
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

	addAndMakeVisible(inGainSlider);
	addAndMakeVisible(outGainSlider);
    addAndMakeVisible(feedbackSlider);
    addAndMakeVisible(feedbackDelaySlider);
}

IgnitiveAudioProcessorEditor::~IgnitiveAudioProcessorEditor() { }

void IgnitiveAudioProcessorEditor::paint (juce::Graphics& g) {
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void IgnitiveAudioProcessorEditor::resized() {
	inGainSlider.setBounds(10, 10, getWidth() - 20, 20);
	outGainSlider.setBounds(10, 40, getWidth() - 20, 20);

	feedbackSlider.setBounds(10, 100, getWidth() - 20, 20);
    feedbackDelaySlider.setBounds(10, 130, getWidth() - 20, 20);
}

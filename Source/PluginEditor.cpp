#include "PluginProcessor.h"
#include "PluginEditor.h"

IgnitiveAudioProcessorEditor::IgnitiveAudioProcessorEditor (IgnitiveAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
    {
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 1000);

	// ==============// GAIN //==============//
    inGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    inGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	inGainSlider.setLookAndFeel(&lookAndFeel);
	addAndMakeVisible(inGainSlider);

    mixSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(mixSlider);

    outGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    outGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    outGainSlider.setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(outGainSlider);

    // ==============// DISTORTION + FEEDBACK //==============//
    driveSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    driveSlider.setLookAndFeel(&driveLAF);
    addAndMakeVisible(driveSlider);

    colorSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    colorSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    colorSlider.setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(colorSlider);

    distortionTypeSelector.addItem("Hard Clip", 1);
    distortionTypeSelector.addItem("Tube", 2);
    distortionTypeSelector.addItem("Fuzz", 3);
    distortionTypeSelector.addItem("Rectify", 4);
    distortionTypeSelector.addItem("Downsample", 5);
    addAndMakeVisible(distortionTypeSelector);

    feedbackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedbackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    feedbackSlider.setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(feedbackSlider);

    feedbackDelaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedbackDelaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    feedbackDelaySlider.setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(feedbackDelaySlider);

    // ==============// FILTERS //==============//

    addAndMakeVisible(preFilterComponent);
    addChildComponent(postFilterComponent);

    addAndMakeVisible(filterToggleButton);
    filterToggleButton.onClick = [this] {
        showingPre = !showingPre;

        preFilterComponent.setVisible(showingPre);
        postFilterComponent.setVisible(!showingPre);

        filterToggleButton.setButtonText(showingPre ? "PRE" : "POST");
        resized();
    };

}

IgnitiveAudioProcessorEditor::~IgnitiveAudioProcessorEditor() {
}

void IgnitiveAudioProcessorEditor::paint (juce::Graphics& g) {
    g.fillAll (juce::Colours::grey);
    

    //==============// --- //==============//
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(0, 0, getWidth(), 100);
    g.fillRect(0, 700, getWidth(), 300);
    g.fillRect(500, 0, 100, 200);


    //==============// SCREENS //==============//
    g.setColour(juce::Colours::black);
    g.fillEllipse(300 - 125, 400 - 125, 250, 250);
    g.fillRect(20, 720, 335, 160);
    g.fillRect(420, 720, 160, 260);
}

void IgnitiveAudioProcessorEditor::resized() {

    auto area = getLocalBounds();

	inGainSlider.setBounds(25, 25, 50, 50);
    mixSlider.setBounds(350, 25, 50, 50);
	outGainSlider.setBounds(425, 25, 50, 50);

    driveSlider.setBounds(175, 275, 250, 250);
    colorSlider.setBounds(175-25, 500, 75, 75);
    distortionTypeSelector.setBounds(440, 375, 140, 50);

	feedbackSlider.setBounds(450, 550, 100, 100);
    feedbackDelaySlider.setBounds(500, 490, 50, 50);

    filterToggleButton.setBounds(200, 125, 50, 25);

    preFilterComponent.setBounds(area);
    postFilterComponent.setBounds(area);
}

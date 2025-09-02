#include "PluginProcessor.h"
#include "PluginEditor.h"

IgnitiveAudioProcessorEditor::IgnitiveAudioProcessorEditor (IgnitiveAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p) {
    setSize (480, 800);

	backgroundImage = juce::ImageCache::getFromMemory(BinaryData::Ignitive_png, BinaryData::Ignitive_pngSize);

    // ==============// FILTERS //==============//

    addAndMakeVisible(preFilterComponent);
    addChildComponent(postFilterComponent);

    addAndMakeVisible(filterToggleButton);
    filterToggleButton.onClick = [this] {
        bool showingPre = filterToggleButton.getIndex();

        preFilterComponent.setVisible(showingPre);
        postFilterComponent.setVisible(!showingPre);

        resized();
    };

	// ==============// GAIN //==============//
    inGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    inGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	inGainSlider.setLookAndFeel(&knobLAF);
	addAndMakeVisible(inGainSlider);

    mixSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setLookAndFeel(&knobLAF);
    addAndMakeVisible(mixSlider);

    outGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    outGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    outGainSlider.setLookAndFeel(&knobLAF);
    addAndMakeVisible(outGainSlider);

    // ==============// DISTORTION + FEEDBACK //==============//
    driveSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    driveSlider.setLookAndFeel(&driveLAF);
    addAndMakeVisible(driveSlider);

    colorSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    colorSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    colorSlider.setLookAndFeel(&knobLAF);
    addAndMakeVisible(colorSlider);

    distortionTypeSelector.addItem("Hard Clip", 1);
    distortionTypeSelector.addItem("Tube", 2);
    distortionTypeSelector.addItem("Fuzz", 3);
    distortionTypeSelector.addItem("Rectify", 4);
    distortionTypeSelector.addItem("Downsample", 5);
    addAndMakeVisible(distortionTypeSelector);

    feedbackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedbackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    feedbackSlider.setLookAndFeel(&knobLAF);
    addAndMakeVisible(feedbackSlider);

    feedbackDelaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedbackDelaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    feedbackDelaySlider.setLookAndFeel(&knobLAF);
    addAndMakeVisible(feedbackDelaySlider);

    // ==============// ENV + LFO //==============//

    attackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    attackSlider.setLookAndFeel(&knobLAF);
    addAndMakeVisible(attackSlider);

    decaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    decaySlider.setLookAndFeel(&knobLAF);
	addAndMakeVisible(decaySlider);

    gateSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gateSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    gateSlider.setLookAndFeel(&knobLAF);
	addAndMakeVisible(gateSlider);

    addAndMakeVisible(envLFOToggleButton);
    envLFOToggleButton.onClick = [this] {
        bool showingEnv = envLFOToggleButton.getIndex() == 1;

        attackSlider.setVisible(showingEnv);
        decaySlider.setVisible(showingEnv);
		gateSlider.setVisible(showingEnv);

        resized();
    };

	addAndMakeVisible(envBox);
}

IgnitiveAudioProcessorEditor::~IgnitiveAudioProcessorEditor() {
}

void IgnitiveAudioProcessorEditor::paint (juce::Graphics& g) {
    if (backgroundImage.isValid()) g.drawImage(backgroundImage, getLocalBounds().toFloat());
    else g.fillAll(juce::Colours::grey);

	juce::Rectangle<int> envBox(30, 590, 240, 120);
}

void IgnitiveAudioProcessorEditor::resized() {
    auto area = getLocalBounds();

    preFilterComponent.setBounds(area);
    postFilterComponent.setBounds(area);

    inGainSlider.setBounds(20, 20, 40, 40);
    mixSlider.setBounds(280, 20, 40, 40);
    outGainSlider.setBounds(340, 20, 40, 40);

    driveSlider.setBounds(140, 220, 200, 200);
    colorSlider.setBounds(103, 398, 60, 60);
    distortionTypeSelector.setBounds(352, 300, 112, 40);

    feedbackSlider.setBounds(360, 440, 80, 80);
    feedbackDelaySlider.setBounds(400, 392, 40, 40);

    filterToggleButton.setBounds(184, 88, 40, 20);

    attackSlider.setBounds(20, 740, 40, 40);
    decaySlider.setBounds(100, 740, 40, 40);
    gateSlider.setBounds(180, 740, 40, 40);

    envLFOToggleButton.setBounds(280, 620, 20, 40);
	envBox.setBounds(35, 590, 230, 120);
}

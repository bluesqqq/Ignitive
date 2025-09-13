#include "PluginProcessor.h"
#include "PluginEditor.h"

IgnitiveAudioProcessorEditor::IgnitiveAudioProcessorEditor (IgnitiveAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor(p), envBox(p), modMatrixComponent(p.ignitive.modMatrix){
    setSize (480, 800);

	backgroundImage = juce::ImageCache::getFromMemory(BinaryData::Ignitive_png, BinaryData::Ignitive_pngSize);

    // ==============// FILTERS //==============//

    addAndMakeVisible(preFilterComponent);
    addChildComponent(postFilterComponent);

    // ==============// MOD MATRIX //==============//
    addAndMakeVisible(modMatrixComponent);

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
	inGainSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
	addAndMakeVisible(inGainSlider);

    mixSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setLookAndFeel(&knobLAF);
    mixSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(mixSlider);

    outGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    outGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    outGainSlider.setLookAndFeel(&knobLAF);
    outGainSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(outGainSlider);

    // ==============// DISTORTION + FEEDBACK //==============//
    addAndMakeVisible(driveKnob);

    colorSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    colorSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    colorSlider.setLookAndFeel(&knobLAF);
    colorSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(colorSlider);

    auto* parameter = audioProcessor.parameters.getParameter(Parameters::ID_DISTORTION_TYPE);
    distortionTypeSelector.addItemList(parameter->getAllValueStrings(), 1);
    distortionTypeSelector.setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
    distortionTypeSelector.setLookAndFeel(&distortionLAF);
    addAndMakeVisible(distortionTypeSelector);
    distortionTypeAttach.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.parameters, Parameters::ID_DISTORTION_TYPE, distortionTypeSelector));

    feedbackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedbackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    feedbackSlider.setLookAndFeel(&knobLAF);
    feedbackSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(feedbackSlider);

    feedbackDelaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedbackDelaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    feedbackDelaySlider.setLookAndFeel(&knobLAF);
    feedbackDelaySlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(feedbackDelaySlider);

    // ==============// ENV + LFO //==============//

    attackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    attackSlider.setLookAndFeel(&knobLAF);
    attackSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(attackSlider);

    decaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    decaySlider.setLookAndFeel(&knobLAF);
    decaySlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
	addAndMakeVisible(decaySlider);

    gateSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gateSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    gateSlider.setLookAndFeel(&knobLAF);
    gateSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
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

	g.drawText("Number of Connections: " + juce::String(audioProcessor.ignitive.modMatrix.getNumOfConnections()), 10, 10, 200, 20, juce::Justification::left);
}

void IgnitiveAudioProcessorEditor::resized() {
    auto area = getLocalBounds();

    preFilterComponent.setBounds(area);
    postFilterComponent.setBounds(area);

    modMatrixComponent.setBounds(260, 580, 200, 200);

    inGainSlider.setBounds(20, 20, 40, 40);
    mixSlider.setBounds(280, 20, 40, 40);
    outGainSlider.setBounds(340, 20, 40, 40);

    driveKnob.setBounds(140, 220, 200, 200);
    colorSlider.setBounds(103, 398, 60, 60);
    distortionTypeSelector.setBounds(352, 300, 112, 40);

    feedbackSlider.setBounds(360, 440, 80, 80);
    feedbackDelaySlider.setBounds(400, 392, 40, 40);

    filterToggleButton.setBounds(184, 88, 40, 20);

    attackSlider.setBounds(20, 740, 40, 40);
    decaySlider.setBounds(100, 740, 40, 40);
    gateSlider.setBounds(180, 740, 40, 40);

    envLFOToggleButton.setBounds(230, 620, 20, 40);
	envBox.setBounds(30, 590, 180, 120);
}

#include "PluginProcessor.h"
#include "PluginEditor.h"

IgnitiveAudioProcessorEditor::IgnitiveAudioProcessorEditor(IgnitiveAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), envBox(p), modMatrixComponent(p.ignitive.modMatrix), birdsEyeLAF(p.ignitive.distortion),
    lfoBox(p),
    digitalFont(juce::Typeface::createSystemTypefaceFor(BinaryData::digital_ttf, BinaryData::digital_ttfSize)),
    uavosdFont(juce::Typeface::createSystemTypefaceFor(BinaryData::uavosd_ttf, BinaryData::uavosd_ttfSize)),
    inMeter(p.ignitive.inGain), outMeter(p.ignitive.outGain),
    filterComponent(p.parameters, p.ignitive.filter, ignitiveLAF) {
    setSize (480, 800);

	backgroundImage = juce::ImageCache::getFromMemory(BinaryData::Ignitive_png, BinaryData::Ignitive_pngSize);

    // ==============// FILTERS //==============//
    addAndMakeVisible(filterComponent);

    // ==============// MOD MATRIX //==============//
    addAndMakeVisible(modMatrixComponent);

    modMatrixViewport.setScrollBarThickness(10);
    modMatrixViewport.setLookAndFeel(&ignitiveLAF);
    modMatrixViewport.setViewedComponent(&modMatrixComponent, false);
    addAndMakeVisible(modMatrixViewport);

	// ==============// GAIN //==============//
    inGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    inGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	inGainSlider.setLookAndFeel(&ignitiveLAF);
	inGainSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
	addAndMakeVisible(inGainSlider);

    mixSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setLookAndFeel(&mixLAF);
    addAndMakeVisible(mixSlider);

    outGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    outGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    outGainSlider.setLookAndFeel(&ignitiveLAF);
    outGainSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(outGainSlider);

    bypassButton.setLookAndFeel(&ignitiveLAF);
    bypassButton.setButtonText("BYPASS");
    addAndMakeVisible(bypassButton);

    oversampleButton.setLookAndFeel(&ignitiveLAF);
    oversampleButton.setButtonText("OVERSAMPLE");
    addAndMakeVisible(oversampleButton);

    limiterButton.setLookAndFeel(&ignitiveLAF);
    limiterButton.setButtonText("LIMITER");
    addAndMakeVisible(limiterButton);

    // ==============// DISTORTION + FEEDBACK //==============//
    addAndMakeVisible(driveKnob);

    colorSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    colorSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    colorSlider.setLookAndFeel(&birdsEyeLAF);
    colorSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(colorSlider);

    auto* distTypeParameter = audioProcessor.parameters.getParameter(Parameters::ID_DISTORTION_TYPE);
    distortionTypeSelector.addItemList(distTypeParameter->getAllValueStrings(), 1);
    distortionTypeSelector.setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
    distortionTypeSelector.setLookAndFeel(&ignitiveLAF);
    addAndMakeVisible(distortionTypeSelector);
    distortionTypeAttach.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.parameters, Parameters::ID_DISTORTION_TYPE, distortionTypeSelector));

    auto* charTypeParameter = audioProcessor.parameters.getParameter(Parameters::ID_CHARACTER_TYPE);
    characterTypeSelector.addItemList(charTypeParameter->getAllValueStrings(), 1);
    characterTypeSelector.setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
    characterTypeSelector.setLookAndFeel(&ignitiveLAF);
    addAndMakeVisible(characterTypeSelector);
    characterTypeAttach.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.parameters, Parameters::ID_CHARACTER_TYPE, characterTypeSelector));



    feedbackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedbackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    feedbackSlider.setLookAndFeel(&ignitiveLAF);
    feedbackSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(feedbackSlider);

    feedbackDelaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedbackDelaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    feedbackDelaySlider.setLookAndFeel(&ignitiveLAF);
    feedbackDelaySlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(feedbackDelaySlider);

    // ==============// ENV + LFO //==============//

    attackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    attackSlider.setLookAndFeel(&ignitiveLAF);
    attackSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(attackSlider);

    decaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    decaySlider.setLookAndFeel(&ignitiveLAF);
    decaySlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
	addAndMakeVisible(decaySlider);

    gateSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gateSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    gateSlider.setLookAndFeel(&ignitiveLAF);
    gateSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
	addAndMakeVisible(gateSlider);

    addAndMakeVisible(lfoBox);

    lfoSpeedSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    lfoSpeedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lfoSpeedSlider.setLookAndFeel(&ignitiveLAF);
    lfoSpeedSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(lfoSpeedSlider);

    addAndMakeVisible(envLFOToggleButton);
    lfoBox.setVisible(false);
    lfoSpeedSlider.setVisible(false);

    envLFOToggleButton.onClick = [this] {
        showingEnvelope = envLFOToggleButton.getIndex() == 0;

        attackSlider.setVisible(showingEnvelope);
        decaySlider.setVisible(showingEnvelope);
		gateSlider.setVisible(showingEnvelope);
        envBox.setVisible(showingEnvelope);

        lfoBox.setVisible(!showingEnvelope);
        lfoSpeedSlider.setVisible(!showingEnvelope);

        modMatrixComponent.setSourceIDFilter(showingEnvelope ? Parameters::ID_ENV : Parameters::ID_LFO);

        resized();
    };

    modMatrixComponent.setSourceIDFilter(Parameters::ID_ENV);

	addAndMakeVisible(envBox);


    addAndMakeVisible(inMeter);
    addAndMakeVisible(outMeter);

    randomizeButton.setLookAndFeel(&ignitiveLAF);
    addAndMakeVisible(randomizeButton);

    settingsButton.setLookAndFeel(&ignitiveLAF);
    addAndMakeVisible(settingsButton);
}

IgnitiveAudioProcessorEditor::~IgnitiveAudioProcessorEditor() {
}

void IgnitiveAudioProcessorEditor::paint (juce::Graphics& g) {
    if (backgroundImage.isValid()) g.drawImage(backgroundImage, getLocalBounds().toFloat());
    else g.fillAll(juce::Colours::grey);

    digitalFont.setHeight(modParamNamesFontSize);
    g.setFont(digitalFont);
    g.setColour(juce::Colours::yellow);

    const juce::StringArray& usingArray = showingEnvelope ? modParamNamesEnvelope : modParamNamesLFO;

    float sectionWidth = modParamNamesBox.getWidth() / 3.0f;

    juce::Random r;
    juce::String symbols = ".#:_+=@!";

    // Go through the first three elements of the using array
    for (int i = 0; i < modParamNames.size(); ++i) {
        juce::String target = usingArray[i];

        juce::String& current = modParamNames.getReference(i);

        if (current.length() < target.length())
            current = current.paddedRight(target.length(), ' ');
        else if (current.length() > target.length())
            current = current.substring(0, target.length());

        juce::String newString;
        for (int j = 0; j < target.length(); ++j) {
            juce::juce_wchar c = current[j];
            juce::juce_wchar t = target[j];

            if (c != t) {
                int choice = r.nextInt(symbols.length() + 1);
                c = choice < symbols.length() ? symbols[choice] : t;
            }

            newString += juce::String::charToString(c);
        }

        current = newString;

        juce::Rectangle<float> sectionRect(
            modParamNamesBox.getX() + i * sectionWidth,
            modParamNamesBox.getY(),
            sectionWidth,
            modParamNamesBox.getHeight()
        );

        g.drawText(current, sectionRect.toNearestInt(), juce::Justification::centred);
    }
}

void IgnitiveAudioProcessorEditor::resized() {
    auto area = getLocalBounds();

    filterComponent.setBounds(area);

    modMatrixViewport.setBounds(275 + 5, 620 + 5, 190 - 10, 165 - 10);

    modMatrixComponent.setSize(165, 250);

    inGainSlider.setBounds(10, 535, 60, 60);
    mixSlider.setBounds(275, 540, 190, 50);
    outGainSlider.setBounds(200, 535, 60, 60);

    // Distortion
    driveKnob.setBounds(140, 185, 200, 200);
    colorSlider.setBounds(97, 368, 60, 60);
    characterTypeSelector.setBounds(7, 265, 112, 40);
    distortionTypeSelector.setBounds(361, 265, 112, 40);

    // Feedback
    feedbackSlider.setBounds(354, 348, 80, 80);
    feedbackDelaySlider.setBounds(296, 417, 40, 40);

    // LFO + ENV
    envLFOToggleButton.setBounds(237, 640, 20, 40);

    attackSlider.setBounds(30, 710, 40, 40);
    decaySlider.setBounds(97, 710, 40, 40);
    gateSlider.setBounds(165, 710, 40, 40);
	envBox.setBounds(15, 620, 205, 80);

    lfoBox.setBounds(15, 620, 205, 80);

    lfoSpeedSlider.setBounds(30, 710, 40, 40);

    bypassButton.setBounds    (325, 10 - 3,  75,  25 + 3);
    oversampleButton.setBounds(80,  535 - 3, 110, 25 + 3);
    limiterButton.setBounds   (80,  570 - 3, 80,  25 + 3);

    inMeter.setBounds(62, 587, 14, 14);
    outMeter.setBounds(252, 587, 14, 14);

    randomizeButton.setBounds(130, 10 - 3, 25, 25 + 3);
    settingsButton.setBounds(410, 10 - 3, 25, 25 + 3);
}

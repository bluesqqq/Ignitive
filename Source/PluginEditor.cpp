#include "PluginProcessor.h"
#include "PluginEditor.h"

IgnitiveAudioProcessorEditor::IgnitiveAudioProcessorEditor(IgnitiveAudioProcessor& p)
    : AudioProcessorEditor(&p), 
      audioProcessor(p), 
      envBox(p), lfoBox(p),
      filterCurve(p.parameters, p.ignitive.filter, ignitiveLAF),
      modMatrixComponent(p.ignitive.modMatrix), birdsEyeLAF(p.ignitive.distortion),
      digitalFont(juce::Typeface::createSystemTypefaceFor(BinaryData::digital_ttf, BinaryData::digital_ttfSize)),
      uavosdFont(juce::Typeface::createSystemTypefaceFor(BinaryData::uavosd_ttf, BinaryData::uavosd_ttfSize)),
      inMeter(p.ignitive.inGain), outMeter(p.ignitive.outGain),
      paramsDisplay(digitalFont) {

    startTimerHz(60);
    setSize (480, 800);

	backgroundImage = juce::ImageCache::getFromMemory(BinaryData::Ignitive_png, BinaryData::Ignitive_pngSize);

    bypassButton.setLookAndFeel(&ignitiveLAF);
    bypassButton.setButtonText("BYPASS");
    addAndMakeVisible(bypassButton);

    // ==============// Filter //==============//
    lpCutoffKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    lpCutoffKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lpCutoffKnob.setLookAndFeel(&ignitiveLAF);
    lpCutoffKnob.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(lpCutoffKnob);

    lpResonanceKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    lpResonanceKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lpResonanceKnob.setLookAndFeel(&ignitiveLAF);
    lpResonanceKnob.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(lpResonanceKnob);

    hpCutoffKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    hpCutoffKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    hpCutoffKnob.setLookAndFeel(&ignitiveLAF);
    hpCutoffKnob.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(hpCutoffKnob);

    hpResonanceKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    hpResonanceKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    hpResonanceKnob.setLookAndFeel(&ignitiveLAF);
    hpResonanceKnob.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(hpResonanceKnob);

    addAndMakeVisible(filterCurve);

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
    addAndMakeVisible(inMeter);

    mixSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setLookAndFeel(&mixLAF);
    addAndMakeVisible(mixSlider);

    outGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    outGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    outGainSlider.setLookAndFeel(&ignitiveLAF);
    outGainSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(outGainSlider);
    addAndMakeVisible(outMeter);

    oversampleButton.setLookAndFeel(&ignitiveLAF);
    oversampleButton.setButtonText("OVERSAMPLE");
    addAndMakeVisible(oversampleButton);

    limiterButton.setLookAndFeel(&ignitiveLAF);
    limiterButton.setButtonText("LIMITER");
    addAndMakeVisible(limiterButton);

    softClipButton.setLookAndFeel(&ignitiveLAF);
    softClipButton.setButtonText("");
    addAndMakeVisible(softClipButton);

    // ==============// DISTORTION + FEEDBACK //==============//
    addAndMakeVisible(driveKnob);

    characterSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    characterSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    characterSlider.setLookAndFeel(&birdsEyeLAF);
    characterSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    addAndMakeVisible(characterSlider);

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
    addAndMakeVisible(paramsDisplay);
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
    paramsDisplay.setState(showingEnvelope);
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
        paramsDisplay.setState(showingEnvelope);

        resized();
    };

    modMatrixComponent.setSourceIDFilter(Parameters::ID_ENV);

	addAndMakeVisible(envBox);

    randomizeButton.setLookAndFeel(&ignitiveLAF);
    auto randomizeIcon = juce::ImageCache::getFromMemory(BinaryData::random_icon_png, BinaryData::random_icon_pngSize);
    randomizeButton.setImages(true, true, true, randomizeIcon, 1.0f, juce::Colours::black, randomizeIcon, 1.0f, juce::Colours::black, randomizeIcon, 1.0f, juce::Colours::black);
    randomizeButton.onClick = [this]() { audioProcessor.randomize(); };
    addAndMakeVisible(randomizeButton);
    randomizeButton.setBounds(95, 10 - 3, 25, 25 + 3);


    // TODO: this needs to bring up a settings menu
    settingsButton.setLookAndFeel(&ignitiveLAF);
    auto settingsIcon = juce::ImageCache::getFromMemory(BinaryData::settings_icon_png, BinaryData::settings_icon_pngSize);
    settingsButton.setImages(true, true, true, settingsIcon, 1.0f, juce::Colours::black, settingsIcon, 1.0f, juce::Colours::black, settingsIcon, 1.0f, juce::Colours::black);
    addAndMakeVisible(settingsButton);
    settingsButton.setBounds(410, 10 - 3, 25, 25 + 3);

    // ==============// PRESETS //==============//
    saveButton.setLookAndFeel(&ignitiveLAF);
    auto saveIcon = juce::ImageCache::getFromMemory(BinaryData::save_icon_png, BinaryData::save_icon_pngSize);
    saveButton.setImages(true, true, true, saveIcon, 1.0f, juce::Colours::black, saveIcon, 1.0f, juce::Colours::black, saveIcon, 1.0f, juce::Colours::black);
    saveButton.onClick = [this]() { audioProcessor.savePreset(); };
    addAndMakeVisible(saveButton);
    saveButton.setBounds(130, 10 - 3, 25, 25 + 3);

    presetSelector.setLookAndFeel(&ignitiveLAF);
    presetSelector.setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(presetSelector);

    presetSelector.onChange = [this]() {
        int selectedID = presetSelector.getSelectedId();
        if (selectedID > 0 && selectedID <= audioProcessor.presets.size()) {
            if (audioProcessor.loadPreset(selectedID - 1))
                // I'm using this to reload it, maybe ill just make the reload public eventually
                modMatrixComponent.setSourceIDFilter(showingEnvelope ? Parameters::ID_ENV : Parameters::ID_LFO);
        }
    };

    int itemID = 1;
    for (auto& preset : audioProcessor.presets) {
        presetSelector.addItem(preset->getName(), itemID++);
    }

    presetSelector.setText("No Preset Selected", false);
}

IgnitiveAudioProcessorEditor::~IgnitiveAudioProcessorEditor() {
}

void IgnitiveAudioProcessorEditor::paint (juce::Graphics& g) {
    if (backgroundImage.isValid()) g.drawImage(backgroundImage, getLocalBounds().toFloat());
    else g.fillAll(juce::Colours::grey);
}

void IgnitiveAudioProcessorEditor::timerCallback() {
    characterSlider.repaint();
    driveKnob.repaint();
    filterCurve.repaint();
    if (showingEnvelope) {
        envBox.repaint();
    } else {
        lfoBox.repaint();
    }
    paramsDisplay.repaint();
}

void IgnitiveAudioProcessorEditor::resized() {
    // ========/ Header Panel /========
    randomizeButton.setBounds(95, 10 - 3, 25, 25 + 3);
    saveButton.setBounds(130, 10 - 3, 25, 25 + 3);
    presetSelector.setBounds(165, 10, 150, 25);
    settingsButton.setBounds(410, 10 - 3, 25, 25 + 3);
    bypassButton.setBounds(325, 10 - 3, 75, 25 + 3);

    // =========/ Main Panel /=========

    // Filter
    hpCutoffKnob.setBounds(40, 95, 60, 60);
    hpResonanceKnob.setBounds(40, 185, 40, 40);
    lpCutoffKnob.setBounds(380, 95, 60, 60);
    lpResonanceKnob.setBounds(400, 185, 40, 40);

    filterCurve.setBounds(110.0f, 85.0f, 260.0f, 80.0f);

    // Distortion
    driveKnob.setBounds(140, 185, 200, 200);
    characterSlider.setBounds(97, 368, 60, 60);
    characterTypeSelector.setBounds(7, 265, 112, 40);
    distortionTypeSelector.setBounds(361, 265, 112, 40);

    // Feedback
    feedbackSlider.setBounds(354, 348, 80, 80);
    feedbackDelaySlider.setBounds(296, 417, 40, 40);

    // =========/ Gain Panel /=========
    inGainSlider.setBounds(10, 535, 60, 60);
    inMeter.setBounds(62, 587, 14, 14);

    oversampleButton.setBounds(80, 535 - 3, 110, 25 + 3);
    limiterButton.setBounds(80, 570 - 3, 80, 25 + 3);
    softClipButton.setBounds(165, 570 - 3, 25, 25 + 3);

    outGainSlider.setBounds(200, 535, 60, 60);
    outMeter.setBounds(252, 587, 14, 14);

    mixSlider.setBounds(275, 540, 190, 50);

    // =========/ Mod Panel /=========
    modMatrixViewport.setBounds(275 + 5, 620 + 5, 190 - 10, 165 - 10);
    modMatrixComponent.setSize(165, 250);
    envLFOToggleButton.setBounds(237, 640, 20, 40);

    // ENV
    attackSlider.setBounds(30, 710, 40, 40);
    decaySlider.setBounds(97, 710, 40, 40);
    gateSlider.setBounds(165, 710, 40, 40);
	envBox.setBounds(15, 620, 205, 80);

    // LFO
    lfoBox.setBounds(15, 620, 205, 80);
    lfoSpeedSlider.setBounds(30, 710, 40, 40);

    paramsDisplay.setBounds(15, 760, 205, 25);
}

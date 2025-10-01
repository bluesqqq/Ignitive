#include "PluginProcessor.h"
#include "PluginEditor.h"

IgnitiveAudioProcessorEditor::IgnitiveAudioProcessorEditor(IgnitiveAudioProcessor& p)
    : AudioProcessorEditor(&p), 
      audioProcessor(p), 
	  modSourceGraph(p, nullptr),
      filterCurve(p.parameters, p.ignitive.filter, ignitiveLAF),
      modMatrixComponent(p.ignitive.modMatrix), birdsEyeLAF(p.ignitive.distortion),
      digitalFont(juce::Typeface::createSystemTypefaceFor(BinaryData::digital_ttf, BinaryData::digital_ttfSize)),
      uavosdFont(juce::Typeface::createSystemTypefaceFor(BinaryData::uavosd_ttf, BinaryData::uavosd_ttfSize)),
      inMeter(p.ignitive.inGain), outMeter(p.ignitive.outGain),
      paramsDisplay(digitalFont),
      characterPolarityButton("Character Polarity", digitalFont) {

    // START
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
	lpCutoffKnob.setTooltip("Lowpass filter cutoff frequency");
    addAndMakeVisible(lpCutoffKnob);

    lpResonanceKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    lpResonanceKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lpResonanceKnob.setLookAndFeel(&ignitiveLAF);
    lpResonanceKnob.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    lpResonanceKnob.setTooltip("Lowpass filter resonance");
    addAndMakeVisible(lpResonanceKnob);

    hpCutoffKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    hpCutoffKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    hpCutoffKnob.setLookAndFeel(&ignitiveLAF);
    hpCutoffKnob.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    hpCutoffKnob.setTooltip("Highpass filter cutoff frequency");
    addAndMakeVisible(hpCutoffKnob);

    hpResonanceKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    hpResonanceKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    hpResonanceKnob.setLookAndFeel(&ignitiveLAF);
    hpResonanceKnob.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    hpResonanceKnob.setTooltip("Highpass filter resonance");
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
    inGainSlider.setTooltip("In gain");
	addAndMakeVisible(inGainSlider);
    addAndMakeVisible(inMeter);

    mixSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setLookAndFeel(&mixLAF);
    mixSlider.setTooltip("Dry/wet mix");
    addAndMakeVisible(mixSlider);

    outGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    outGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    outGainSlider.setLookAndFeel(&ignitiveLAF);
    outGainSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    outGainSlider.setTooltip("Out gain");
    addAndMakeVisible(outGainSlider);
    addAndMakeVisible(outMeter);

    oversampleButton.setLookAndFeel(&ignitiveLAF);
    oversampleButton.setButtonText("OVERSAMPLE");
    oversampleButton.setTooltip("Toggle distortion oversampling on/off");
    addAndMakeVisible(oversampleButton);

    limiterButton.setLookAndFeel(&ignitiveLAF);
    limiterButton.setButtonText("LIMITER");
    limiterButton.setTooltip("Toggle limiter on/off");
    addAndMakeVisible(limiterButton);

    softClipButton.setLookAndFeel(&ignitiveLAF);
    softClipButton.setButtonText("");
    softClipButton.setTooltip("unused");
    addAndMakeVisible(softClipButton);

    // ==============// DISTORTION //==============//

    // Drive
    addAndMakeVisible(driveKnob);

    auto distortionMenu = distortionTypeSelector.getRootMenu();
    DistortionProcessor::makeDistortionAlgosMenu(*distortionMenu);

    auto* distTypeParameter = dynamic_cast<juce::AudioParameterChoice*>(audioProcessor.parameters.getParameter(Parameters::ID_DISTORTION_TYPE));
    distortionTypeSelector.setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
    distortionTypeSelector.setLookAndFeel(&ignitiveLAF);
    addAndMakeVisible(distortionTypeSelector);

    /* I'm not using a ComboBoxAttachment here because when connected to a ComboBox, it
    *  updates to the item's index (in order that the menu appears), instead of the ID of
    * the selected item.
    * 
    * Since I'm reordering the distortion algorithms into submenus (see DistortionProcessor::makeDistortionAlgosMenu())
    * I can't guarantee the indexes will line up. Instead, I'm just using the item's ID as
    * the index + 1 (since PopupMenus are 1-indexed in JUCE).
    * 
    * I could have used custom unique ID's for each distortion algorithm, but indexing seemed simpler.
    */
    distortionTypeSelector.onChange = [this, distTypeParameter] {
        auto selectedID = distortionTypeSelector.getSelectedId();
        int index = selectedID - 1;
        if (selectedID >= 0 && selectedID < DistortionProcessor::getNumAlgorithms()) {
            distTypeParameter->operator=(index);
        }
	};

    audioProcessor.parameters.addParameterListener(Parameters::ID_DISTORTION_TYPE, this);
	distortionTypeSelector.setSelectedId(distTypeParameter->getIndex() + 1, juce::dontSendNotification);
    
    // Character 
    characterSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    characterSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    characterSlider.setLookAndFeel(&birdsEyeLAF);
    characterSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    characterSlider.setTooltip("Distortion character amount");
    addAndMakeVisible(characterSlider);

    auto* charTypeParameter = audioProcessor.parameters.getParameter(Parameters::ID_CHARACTER_TYPE);
    characterTypeSelector.addItemList(charTypeParameter->getAllValueStrings(), 1);
    characterTypeSelector.setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
    characterTypeSelector.setLookAndFeel(&ignitiveLAF);
    characterTypeSelector.setTooltip("Distortion character type");
    addAndMakeVisible(characterTypeSelector);
    characterTypeAttach.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.parameters, Parameters::ID_CHARACTER_TYPE, characterTypeSelector));

    characterTypeSelector.onChange = [this] {
		characterPolarityButton.setVisible(DistortionProcessor::CharacterHasPolarity(characterTypeSelector.getSelectedItemIndex()));
    };

    addAndMakeVisible(characterPolarityButton);
    characterPolarityButton.onIndexChange = [this](int newIndex) {
        auto* characterPolarityParameter = dynamic_cast<juce::AudioParameterChoice*>(audioProcessor.parameters.getParameter(Parameters::ID_CHARACTER_POLARITY));
        if (characterPolarityParameter != nullptr) {
            characterPolarityParameter->operator=(newIndex);
        }
	};

    // ==============// Feedback //==============//
    feedbackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedbackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    feedbackSlider.setLookAndFeel(&ignitiveLAF);
    feedbackSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    feedbackSlider.setTooltip("Feedback amount");
    addAndMakeVisible(feedbackSlider);

    feedbackDelaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedbackDelaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    feedbackDelaySlider.setLookAndFeel(&ignitiveLAF);
    feedbackDelaySlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    feedbackSlider.setTooltip("Feedback delay time");
    addAndMakeVisible(feedbackDelaySlider);

    // ==============// ENV + LFO //==============//
    addAndMakeVisible(paramsDisplay);
    attackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    attackSlider.setLookAndFeel(&ignitiveLAF);
    attackSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    attackSlider.setTooltip("Envelope attack time");
    addAndMakeVisible(attackSlider);

    decaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    decaySlider.setLookAndFeel(&ignitiveLAF);
    decaySlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    decaySlider.setTooltip("Envelope decay time");
	addAndMakeVisible(decaySlider);

    gateSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gateSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    gateSlider.setLookAndFeel(&ignitiveLAF);
    gateSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    gateSlider.setTooltip("Envelope gate threshold");
	addAndMakeVisible(gateSlider);

    lfoSpeedSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    lfoSpeedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lfoSpeedSlider.setLookAndFeel(&ignitiveLAF);
    lfoSpeedSlider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    lfoSpeedSlider.setTooltip("LFO speed");
    addAndMakeVisible(lfoSpeedSlider);

    paramsDisplay.setState(showingEnvelope);
    lfoSpeedSlider.setVisible(false);

    envLFOToggleButton.onClick = [this] {
        showingEnvelope = !envLFOToggleButton.getToggleState();

        attackSlider.setVisible(showingEnvelope);
        decaySlider.setVisible(showingEnvelope);
		gateSlider.setVisible(showingEnvelope);
        modSourceGraph.setSource(showingEnvelope
            ? static_cast<ModSource*>(&audioProcessor.ignitive.envelope)
            : static_cast<ModSource*>(&audioProcessor.ignitive.lfo));
        lfoSpeedSlider.setVisible(!showingEnvelope);

        modMatrixComponent.setSourceIDFilter(showingEnvelope ? Parameters::ID_ENV : Parameters::ID_LFO);
        paramsDisplay.setState(showingEnvelope);

        resized();
    };
    envLFOToggleButton.setTooltip("Toggle envelope/lfo display");
	envLFOToggleButton.setLookAndFeel(&switchLAF);
    addAndMakeVisible(envLFOToggleButton);

    modMatrixComponent.setSourceIDFilter(Parameters::ID_ENV);

	addAndMakeVisible(modSourceGraph);

    randomizeButton.setLookAndFeel(&ignitiveLAF);
    auto randomizeIcon = juce::ImageCache::getFromMemory(BinaryData::random_icon_png, BinaryData::random_icon_pngSize);
    randomizeButton.setImages(true, true, true, randomizeIcon, 1.0f, juce::Colours::black, randomizeIcon, 1.0f, juce::Colours::black, randomizeIcon, 1.0f, juce::Colours::black);
    randomizeButton.onClick = [this]() {
        audioProcessor.randomize();
        modMatrixComponent.rebuildSlots();
    };
    randomizeButton.setTooltip("Randomize parameters");
    addAndMakeVisible(randomizeButton);
    randomizeButton.setBounds(95, 10 - 3, 25, 25 + 3);


    // TODO: this needs to bring up a settings menu
    settingsButton.setLookAndFeel(&ignitiveLAF);
    auto settingsIcon = juce::ImageCache::getFromMemory(BinaryData::settings_icon_png, BinaryData::settings_icon_pngSize);
    settingsButton.setImages(true, true, true, settingsIcon, 1.0f, juce::Colours::black, settingsIcon, 1.0f, juce::Colours::black, settingsIcon, 1.0f, juce::Colours::black);
    settingsButton.setTooltip("Open settings panel");
    addAndMakeVisible(settingsButton);
    settingsButton.setBounds(410, 10 - 3, 25, 25 + 3);

    // ==============// PRESETS //==============//
    saveButton.setLookAndFeel(&ignitiveLAF);
    auto saveIcon = juce::ImageCache::getFromMemory(BinaryData::save_icon_png, BinaryData::save_icon_pngSize);
    saveButton.setImages(true, true, true, saveIcon, 1.0f, juce::Colours::black, saveIcon, 1.0f, juce::Colours::black, saveIcon, 1.0f, juce::Colours::black);
    saveButton.onClick = [this]() { audioProcessor.savePreset(); };
    saveButton.setTooltip("Save current patch as preset");
    addAndMakeVisible(saveButton);
    saveButton.setBounds(130, 10 - 3, 25, 25 + 3);

    presetSelector.setLookAndFeel(&ignitiveLAF);
    presetSelector.setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
    presetSelector.setTooltip("Select preset");
    addAndMakeVisible(presetSelector);

    presetSelector.onChange = [this]() {
        int selectedID = presetSelector.getSelectedId();
        if (selectedID > 0 && selectedID <= audioProcessor.presets.size()) {
            if (audioProcessor.loadPreset(selectedID - 1)) {
                modMatrixComponent.rebuildSlots();
            }
        }
    };

    int itemID = 1;
    for (auto& preset : audioProcessor.presets) {
        presetSelector.addItem(preset->getName(), itemID++);
    }

    presetSelector.setText("No Preset", false);
}

IgnitiveAudioProcessorEditor::~IgnitiveAudioProcessorEditor() {
    audioProcessor.parameters.removeParameterListener(Parameters::ID_DISTORTION_TYPE, this);
}

void IgnitiveAudioProcessorEditor::paint (juce::Graphics& g) {
    if (backgroundImage.isValid()) g.drawImage(backgroundImage, getLocalBounds().toFloat());
    else g.fillAll(juce::Colours::grey);
}

void IgnitiveAudioProcessorEditor::timerCallback() {
    characterSlider.repaint();
    driveKnob.repaint();
    filterCurve.repaint();
	modSourceGraph.repaint();
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
    characterPolarityButton.setBounds(7, 313, 35, 35);

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

    // LFO
    lfoSpeedSlider.setBounds(30, 710, 40, 40);

    modSourceGraph.setBounds(15, 620, 205, 80);

    paramsDisplay.setBounds(15, 760, 205, 25);
}

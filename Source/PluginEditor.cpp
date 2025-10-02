#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Globals.h"

void IgnitiveAudioProcessorEditor::parameterChanged(const juce::String& parameterID, float newValue) {
    if (parameterID == Parameters::ID_DISTORTION_TYPE) {
        auto* distTypeParameter = dynamic_cast<juce::AudioParameterChoice*>(audioProcessor.parameters.getParameter(Parameters::ID_DISTORTION_TYPE));

        if (distTypeParameter != nullptr) {
            int index = distTypeParameter->getIndex();
            juce::MessageManager::callAsync([this, index]() { distortionTypeSelector.setSelectedId(index + 1, juce::dontSendNotification); });
        }
    }
}

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
    startTimerHz(Globals::frameRate);
    setSize (Globals::windowWidth, Globals::windowHeight);

	backgroundImage = juce::ImageCache::getFromMemory(BinaryData::Ignitive_png, BinaryData::Ignitive_pngSize);

    // =============== [ HELPERS ] =============== //

    auto setupRotarySlider = [this](juce::Slider& slider, juce::LookAndFeel* laf, const char* tooltip) {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setLookAndFeel(laf);
        slider.setRotaryParameters(Globals::rotaryStartAngle, Globals::rotaryEndAngle, true);
        slider.setTooltip(tooltip);
        addAndMakeVisible(slider);
    };

    auto setupButton = [this](juce::Button& button, juce::LookAndFeel* laf, const juce::String& text, const char* tooltip) {
        button.setLookAndFeel(laf);
        button.setButtonText(text);
        button.setTooltip(tooltip);
        addAndMakeVisible(button);
    };

    auto setupImageButton = [this](juce::ImageButton& button, juce::LookAndFeel* laf, const void* imageData, size_t imageSize, const char* tooltip, std::function<void()> onClick) {
        button.setLookAndFeel(laf);
        auto icon = juce::ImageCache::getFromMemory(imageData, imageSize);
        button.setImages(true, true, true, icon, 1.0f, juce::Colours::black, icon, 1.0f, juce::Colours::black, icon, 1.0f, juce::Colours::black);
        button.setTooltip(tooltip);
        button.onClick = std::move(onClick);
        addAndMakeVisible(button);
    };

    // =============== [ HEADER ] =============== //

    // Randomize
    setupImageButton(randomizeButton, &ignitiveLAF, BinaryData::random_icon_png, BinaryData::random_icon_pngSize, Globals::tooltipRandomize, [this]() {
        audioProcessor.randomize();
        modMatrixComponent.rebuildSlots();
    });

    // Save
    setupImageButton(saveButton, &ignitiveLAF, BinaryData::save_icon_png, BinaryData::save_icon_pngSize, Globals::tooltipSavePreset, [this]() {
        audioProcessor.savePreset();
    });

    // Preset Selector
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
    presetSelector.setText(audioProcessor.currentPresetName, false);

    // Bypass
    setupButton(bypassButton, &ignitiveLAF, "BYPASS", Globals::tooltipBypass);

    // Settings
    setupImageButton(settingsButton, &ignitiveLAF, BinaryData::settings_icon_png, BinaryData::settings_icon_pngSize, Globals::tooltipSettings, [this]() {
        // TODO: open settings menu
    });

    // =============== [ MAIN PANEL ] =============== //
    
    // Filter
    setupRotarySlider(lpCutoffKnob, &ignitiveLAF, Globals::tooltipLowpassCutoff);
    setupRotarySlider(lpResonanceKnob, &ignitiveLAF, Globals::tooltipLowpassResonance);
    setupRotarySlider(hpCutoffKnob, &ignitiveLAF, Globals::tooltipHighpassCutoff);
    setupRotarySlider(hpResonanceKnob, &ignitiveLAF, Globals::tooltipHighpassResonance);
    addAndMakeVisible(filterCurve);

    // Distortion
    addAndMakeVisible(driveKnob);

    auto distortionMenu = distortionTypeSelector.getRootMenu();
    DistortionProcessor::makeDistortionAlgosMenu(*distortionMenu);

    auto* distTypeParameter = dynamic_cast<juce::AudioParameterChoice*>(audioProcessor.parameters.getParameter(Parameters::ID_DISTORTION_TYPE));
    distortionTypeSelector.setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
    distortionTypeSelector.setLookAndFeel(&ignitiveLAF);
    distortionTypeSelector.setTooltip(Globals::tooltipDistortionType);
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
    setupRotarySlider(characterSlider, &birdsEyeLAF, Globals::tooltipDistortionCharacter);

    auto* charTypeParameter = audioProcessor.parameters.getParameter(Parameters::ID_CHARACTER_TYPE);
    characterTypeSelector.addItemList(charTypeParameter->getAllValueStrings(), 1);
    characterTypeSelector.setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
    characterTypeSelector.setLookAndFeel(&ignitiveLAF);
    characterTypeSelector.setTooltip(Globals::tooltipDistortionCharType);
    addAndMakeVisible(characterTypeSelector);
    characterTypeAttach.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.parameters, Parameters::ID_CHARACTER_TYPE, characterTypeSelector));

    characterTypeSelector.onChange = [this] {
        characterPolarityButton.setVisible(DistortionProcessor::CharacterHasPolarity(characterTypeSelector.getSelectedItemIndex()));
    };

    characterPolarityButton.setTooltip(Globals::tooltipDistortionCharPol);
    addAndMakeVisible(characterPolarityButton);

    // Feedback
    setupRotarySlider(feedbackSlider, &ignitiveLAF, Globals::tooltipFeedback);
    setupRotarySlider(feedbackDelaySlider, &ignitiveLAF, Globals::tooltipFeedbackDelay);

    // =============== [ GAIN PANEL ] =============== //

    // Gain
    setupRotarySlider(inGainSlider, &ignitiveLAF, Globals::tooltipInGain);
    addAndMakeVisible(inMeter);
    setupRotarySlider(outGainSlider, &ignitiveLAF, Globals::tooltipOutGain);
    addAndMakeVisible(outMeter);

	// Oversample & Limiter
    setupButton(oversampleButton, &ignitiveLAF, "OVERSAMPLE", Globals::tooltipOversample);
    setupButton(limiterButton, &ignitiveLAF, "LIMITER", Globals::tooltipLimiter);

    // Mix
    mixSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setLookAndFeel(&mixLAF);
    mixSlider.setTooltip(Globals::tooltipMix);
    addAndMakeVisible(mixSlider);

    // =============== [ MODULATION PANEL ] =============== //

    addAndMakeVisible(modMatrixComponent); // This needs to come before viewport or else glitches happen
    modMatrixComponent.setSourceIDFilter(Parameters::ID_ENV);

    modMatrixViewport.setScrollBarThickness(10);
    modMatrixViewport.setLookAndFeel(&ignitiveLAF);
    modMatrixViewport.setViewedComponent(&modMatrixComponent, false);
    addAndMakeVisible(modMatrixViewport);

    // Envelope
    setupRotarySlider(attackSlider, &ignitiveLAF, Globals::tooltipEnvAttack);
    setupRotarySlider(decaySlider, &ignitiveLAF, Globals::tooltipEnvDecay);
    setupRotarySlider(gateSlider, &ignitiveLAF, Globals::tooltipEnvGate);
	attackSlider.setVisible(showingEnvelope);
	decaySlider.setVisible(showingEnvelope);
	gateSlider.setVisible(showingEnvelope);

    // LFO
    setupRotarySlider(lfoSpeedSlider, &ignitiveLAF, Globals::tooltipLfoSpeed);
	lfoSpeedSlider.setVisible(!showingEnvelope);

	// Env / LFO Toggle
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
		paramsDisplay.setInterface(showingEnvelope ? static_cast<ParametersDisplayInterface*>(&audioProcessor.ignitive.envelope) : static_cast<ParametersDisplayInterface*>(&audioProcessor.ignitive.lfo));

        resized();
    };
    envLFOToggleButton.setTooltip(Globals::tooltipEnvLfoToggle);
	envLFOToggleButton.setLookAndFeel(&switchLAF);
    addAndMakeVisible(envLFOToggleButton);

    // Mod Source
    modSourceGraph.setSource(showingEnvelope ? static_cast<ModSource*>(&audioProcessor.ignitive.envelope) : static_cast<ModSource*>(&audioProcessor.ignitive.lfo));
	addAndMakeVisible(modSourceGraph);

    // Parameters Display
    addAndMakeVisible(paramsDisplay);
    paramsDisplay.setInterface(static_cast<ParametersDisplayInterface*>(&audioProcessor.ignitive.envelope));

    attackSlider.onDragStart = [this] { paramsDisplay.showValue(0); };
    decaySlider.onDragStart  = [this] { paramsDisplay.showValue(1); };
    gateSlider.onDragStart   = [this] { paramsDisplay.showValue(2); };

    attackSlider.onDragEnd = [this] { paramsDisplay.hideValue(0); };
    decaySlider.onDragEnd  = [this] { paramsDisplay.hideValue(1); };
    gateSlider.onDragEnd   = [this] { paramsDisplay.hideValue(2); };

    lfoSpeedSlider.onDragStart = [this] { paramsDisplay.showValue(0); };
    lfoSpeedSlider.onDragEnd = [this]   { paramsDisplay.hideValue(0); };
}

IgnitiveAudioProcessorEditor::~IgnitiveAudioProcessorEditor() {
    audioProcessor.parameters.removeParameterListener(Parameters::ID_DISTORTION_TYPE, this);
}

void IgnitiveAudioProcessorEditor::paint (juce::Graphics& g) {
    if (backgroundImage.isValid()) g.drawImage(backgroundImage, getLocalBounds().toFloat());
    else g.fillAll(juce::Colours::grey);

    g.setFont(uavosdFont.withHeight(12.0f));
	g.setColour(juce::Colours::black);

    juce::Rectangle<float> lfoTextBounds = { 237 - 15, 640 + 40, 20 + 25, 25 };
    g.drawText("LFO", lfoTextBounds, juce::Justification::centred);

    juce::Rectangle<float> envTextBounds = { 237 - 15, 640 - 30, 20 + 25, 25 };
    g.drawText("ENV", envTextBounds, juce::Justification::centred);
}

void IgnitiveAudioProcessorEditor::timerCallback() {
    characterSlider.repaint();
    driveKnob.repaint();
    filterCurve.repaint();
	modSourceGraph.repaint();
    paramsDisplay.repaint();

    envLFOToggleButton.repaint();
}

void IgnitiveAudioProcessorEditor::resized() {
    randomizeButton.setBounds(Globals::randomizeButtonBounds);
    saveButton.setBounds(Globals::saveButtonBounds);
    presetSelector.setBounds(Globals::presetSelectorBounds);
    settingsButton.setBounds(Globals::settingsButtonBounds);
    bypassButton.setBounds(Globals::bypassButtonBounds);

    hpCutoffKnob.setBounds(Globals::hpCutoffKnobBounds);
    hpResonanceKnob.setBounds(Globals::hpResonanceKnobBounds);
    lpCutoffKnob.setBounds(Globals::lpCutoffKnobBounds);
    lpResonanceKnob.setBounds(Globals::lpResonanceKnobBounds);

    filterCurve.setBounds(Globals::filterCurveBounds);

    driveKnob.setBounds(Globals::driveKnobBounds);
    characterSlider.setBounds(Globals::characterSliderBounds);
    characterTypeSelector.setBounds(Globals::characterTypeSelectorBounds);
    distortionTypeSelector.setBounds(Globals::distortionTypeSelectorBounds);
    characterPolarityButton.setBounds(Globals::characterPolarityButtonBounds);

    feedbackSlider.setBounds(Globals::feedbackSliderBounds);
    feedbackDelaySlider.setBounds(Globals::feedbackDelaySliderBounds);

    inGainSlider.setBounds(Globals::inGainSliderBounds);
    inMeter.setBounds(Globals::inMeterBounds);

    oversampleButton.setBounds(Globals::oversampleButtonBounds);
    limiterButton.setBounds(Globals::limiterButtonBounds);
    outGainSlider.setBounds(Globals::outGainSliderBounds);
    outMeter.setBounds(Globals::outMeterBounds);

    mixSlider.setBounds(Globals::mixSliderBounds);

    modMatrixViewport.setBounds(Globals::modMatrixViewportBounds);
    modMatrixComponent.setSize(Globals::modMatrixComponentSize.getWidth(), Globals::modMatrixComponentSize.getHeight());
    envLFOToggleButton.setBounds(Globals::envLFOToggleButtonBounds);

    attackSlider.setBounds(Globals::attackSliderBounds);
    decaySlider.setBounds(Globals::decaySliderBounds);
    gateSlider.setBounds(Globals::gateSliderBounds);

    lfoSpeedSlider.setBounds(Globals::lfoSpeedSliderBounds);

    modSourceGraph.setBounds(Globals::modSourceGraphBounds);

    paramsDisplay.setBounds(Globals::paramsDisplayBounds);
}


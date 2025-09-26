#pragma once

#include <JuceHeader.h>

class IgnitiveLAF : public juce::LookAndFeel_V4 {
private:
    float buttonHeight = 3.0f;

    juce::Font uavFont;
    juce::Font digitalFont;

    juce::Colour popupBackground = juce::Colour::fromRGB(12, 13, 16);
    juce::Colour popupStroke = juce::Colours::black;

public:
    IgnitiveLAF(const juce::Font& uavFont, const juce::Font& digitalFont) : uavFont(uavFont), digitalFont(digitalFont) {}
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& toggleButton, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& comboBox);

    void drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollBar, int x, int y, int width, int height, bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown) override;

    void drawPopupMenuBackground(juce::Graphics&, int width, int height) override;

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
        const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* textColour) override;

    void drawImageButton(juce::Graphics& g, juce::Image* image, int imageX, int imageY, int imageW, int imageH, const juce::Colour& overlayColour, float imageOpacity, juce::ImageButton& imageButton) override;
};
#include "IgnitiveLAF.h"

void IgnitiveLAF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) {
    g.setColour(juce::Colours::black);
    g.fillEllipse(x, y, width, height);

    g.setColour(juce::Colours::white);
    g.fillEllipse(x + 2, y + 2, width - 4, height - 4);

    g.setColour(juce::Colours::black);

    float angle = juce::jmap(sliderPos, rotaryStartAngle, rotaryEndAngle) - juce::MathConstants<float>::halfPi;
    float radius = width / 2;
    int centerX = x + width / 2;
    int centerY = y + height / 2;
    g.drawLine(centerX, centerY, centerX + std::cos(angle) * radius, centerY + std::sin(angle) * radius, 3);
}

void IgnitiveLAF::drawToggleButton(juce::Graphics& g, juce::ToggleButton& toggleButton, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
    bool buttonDown = toggleButton.getToggleState();
    auto bounds = toggleButton.getLocalBounds().toFloat();

    auto buttonBase = bounds.withTrimmedTop(buttonHeight);

    float currentHeight = shouldDrawButtonAsDown ? -2 : (buttonDown ? 0 : buttonHeight);

    auto buttonTop = buttonBase.translated(0, -currentHeight).getIntersection(bounds);

    // Base
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(buttonBase, 5.0f);

    //Top
    juce::Colour topColor(shouldDrawButtonAsDown ? 0xff666c7b : (buttonDown ? 0xff7b818f : 0xffffffff));
    juce::Colour textColor(shouldDrawButtonAsDown || buttonDown ? juce::Colours::red : juce::Colours::black);

    g.setColour(topColor);
    g.fillRoundedRectangle(buttonTop, 5.0f);

    g.setFont(uavFont.withHeight(11.0f));

    g.setColour(textColor);
    g.drawText(toggleButton.getButtonText(), buttonTop, juce::Justification::centred);
    buttonTop.reduce(1.0f, 1.0f);

    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(buttonTop, 4.0f, 2.1f);
}

void IgnitiveLAF::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
    auto bounds = button.getLocalBounds().toFloat();

    auto buttonBase = bounds.withTrimmedTop(buttonHeight);

    float currentHeight = shouldDrawButtonAsDown ? 1 : buttonHeight;

    auto buttonTop = buttonBase.translated(0, -currentHeight).getIntersection(bounds);

    // Base
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(buttonBase, 5.0f);

    // Top
    juce::Colour topColor(shouldDrawButtonAsDown ? 0xff7b818f : 0xffffffff);
    juce::Colour textColor(shouldDrawButtonAsDown ? juce::Colours::red : juce::Colours::black);

    g.setColour(topColor);
    g.fillRoundedRectangle(buttonTop, 5.0f);

    buttonTop.reduce(1.0f, 1.0f);

    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(buttonTop, 4.0f, 2.1f);
}

void IgnitiveLAF::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& comboBox) {
    g.setColour(juce::Colours::red);

    static juce::Font customFont = [] {
        auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::digital_ttf,
            BinaryData::digital_ttfSize);
        return juce::Font(typeface);
        }();

    g.setFont(customFont.withHeight(22.0f));

    g.drawText(comboBox.getText(), comboBox.getLocalBounds().toFloat().reduced(4), juce::Justification::centred);
}

void IgnitiveLAF::drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollBar, int x, int y, int width, int height, bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown) {
    auto bounds = scrollBar.getLocalBounds().toFloat();

    const int pixelSize = 5;
    int wPixels = (int)bounds.getWidth() / pixelSize;

    int thumbStartPixel = thumbStartPosition / pixelSize;
    int thumbSizePixel = thumbSize / pixelSize;
    int thumbEndPixel = thumbStartPixel + thumbSizePixel;

    g.setColour(juce::Colours::yellow);

    // Todo: move this
    juce::Colour backgroundColor = juce::Colour::fromRGB(127, 127, 0);

    for (int ix = 0; ix < wPixels; ix++) {
        for (int iy = thumbStartPixel; iy <= thumbEndPixel; iy++) {
            if (iy == thumbStartPixel || iy == thumbEndPixel) g.setColour(backgroundColor);
            else g.setColour(juce::Colours::yellow);

            juce::Rectangle<float> pixel(bounds.getX() + ix * pixelSize, bounds.getY() + iy * pixelSize, pixelSize - 1.0f, pixelSize - 1.0f);

            g.fillRect(pixel);
        }
    }
}

void IgnitiveLAF::drawPopupMenuBackground(juce::Graphics& g, int width, int height) {
    g.setColour(popupStroke);
    g.fillRoundedRectangle(0, 0, width, height, 5.0f);

    g.setColour(popupBackground);
    g.fillRect(5, 5, width - 10, height - 10);
}

void IgnitiveLAF::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* textColour) {
    g.setFont(digitalFont.withHeight(18.0f));
    g.setColour(juce::Colours::red);
    g.drawText(text, area, juce::Justification::centred);
}

void IgnitiveLAF::drawImageButton(juce::Graphics& g, juce::Image* image, int imageX, int imageY, int imageW, int imageH, const juce::Colour& overlayColour, float imageOpacity, juce::ImageButton& imageButton) {
    auto bounds = imageButton.getLocalBounds().toFloat();

    drawButtonBackground(g, imageButton,
        imageButton.findColour(juce::TextButton::buttonColourId),
        imageButton.isMouseOver(),
        imageButton.isDown());

    if (image == nullptr || !image->isValid()) return;

    if (imageButton.isDown())
        imageY += 2; // push the icon down visually

    g.setOpacity(imageOpacity);
    g.drawImage(*image, imageX, imageY, imageW, imageH, 0, 0, image->getWidth(), image->getHeight());
}

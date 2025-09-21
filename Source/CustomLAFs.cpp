#include "CustomLafs.h"

DriveLAF::DriveLAF(DistortionProcessor& dist) : distortion(dist) {}

void DriveLAF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) {
    float angle = juce::jmap(sliderPos, rotaryStartAngle, rotaryEndAngle);
    int centerX = x + width / 2;
    int centerY = y + height / 2;
    float radius = width / 2 - 8;

    float modifiedValue = distortion.getModifiedDriveValue();
    float modifiedAngle = juce::jmap(modifiedValue, rotaryStartAngle, rotaryEndAngle);

    // Parameter arc
    juce::Path arc;
    arc.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
    g.setColour(juce::Colours::red);
    g.strokePath(arc, juce::PathStrokeType(5, juce::PathStrokeType::mitered, juce::PathStrokeType::butt));

    // Modified arc
    juce::Path modArc;
    modArc.addCentredArc(centerX, centerY, radius, radius, 0.0f, angle, modifiedAngle, true);
    g.setColour(juce::Colours::yellow);
    g.strokePath(modArc, juce::PathStrokeType(5, juce::PathStrokeType::mitered, juce::PathStrokeType::butt));

    // Waveshape
	std::vector<float> waveshape = distortion.getWaveshape(128);

    juce::Path path;

    float border = 20;

	float reducedWidth = width - border * 2;

	reducedWidth = sin(juce::MathConstants<float>::halfPi / 2) * reducedWidth;

	float segmentWidth = reducedWidth / (float)(waveshape.size());

	float start = (width - reducedWidth) / 2;

	float halfHeight = height / 2;

    for (int i = 0; i < waveshape.size(); ++i) {
        float x = start + i * segmentWidth;
        float y = halfHeight - (waveshape[i] * (reducedWidth * 0.5));

        if (i == 0) path.startNewSubPath(x, y);
        else        path.lineTo(x, y);
    }

    g.setColour(juce::Colours::red);
    g.strokePath(path, juce::PathStrokeType(5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void DistortionLAF::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& comboBox) {
    g.setColour(juce::Colours::red);

    static juce::Font customFont = [] {
        auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::digital_ttf,
            BinaryData::digital_ttfSize);
        return juce::Font(typeface);
        }();

    g.setFont(customFont.withHeight(22.0f));

    g.drawText(comboBox.getText(), comboBox.getLocalBounds().toFloat().reduced(4), juce::Justification::centredLeft);
}

void ModSlotLAF::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, juce::Slider::SliderStyle sliderStyle, juce::Slider& slider) {
    juce::Rectangle<float> bounds = slider.getLocalBounds().toFloat().reduced(4.0f);

    float gapSize = 2.0f;
    int segments = 10;

    // Center pivot point
    float centerWidth = 6.0f;

    float gaplessWidth = bounds.getWidth() - (gapSize * (float)segments * 2.0f);
    float segmentWidth = (gaplessWidth - centerWidth) / ((float)segments * 2.0f);

    // Negative values
    for (int i = 0; i < segments; i++) {
        juce::Rectangle<float> segment(bounds.getX() + (segmentWidth + gapSize) * i, bounds.getY(), segmentWidth, bounds.getHeight());
        float value = juce::jmap((float)i, 0.0f, (float)segments - 1.0f, -0.95f, -0.1f);

        if (value >= slider.getValue()) {
            g.setColour(juce::Colours::yellow);
            g.fillRect(segment);
        } else {
            g.setColour(juce::Colour::fromRGB(80, 80, 0));
            g.drawRect(segment);
        }
    }

    // Center
    float centerX = bounds.getX() + (segmentWidth + gapSize) * segments;
    juce::Rectangle<float> center(centerX, bounds.getY(), centerWidth, bounds.getHeight());

    g.setColour(juce::Colours::yellow);
    g.fillRect(center);

    // Positive values
    for (int i = 0; i < segments; i++) {
        juce::Rectangle<float> segment(centerX + centerWidth + gapSize + (segmentWidth + gapSize) * i, bounds.getY(), segmentWidth, bounds.getHeight());
        float value = juce::jmap((float)i, 0.0f, (float)segments - 1.0f, 0.1f, 0.95f);

        if (value <= slider.getValue()) {
            g.setColour(juce::Colours::yellow);
            g.fillRect(segment);
        }
        else {
            g.setColour(juce::Colour::fromRGB(80, 80, 0));
            g.drawRect(segment);
        }
    }
}

void ModSlotLAF::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& comboBox) {
    g.setColour(juce::Colours::yellow);

    static juce::Font customFont = [] {
        auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::digital_ttf,
            BinaryData::digital_ttfSize);
        return juce::Font(typeface);
        }();

    g.setFont(customFont.withHeight(18.0f));

    g.drawText(comboBox.getText(), comboBox.getLocalBounds().toFloat().reduced(4), juce::Justification::centred);
}

void ModSlotLAF::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
    auto bounds = button.getLocalBounds().toFloat();

    float size = std::min(bounds.getWidth(), bounds.getHeight()) * 0.5f; // size of the X square
    auto center = bounds.getCentre();

    float halfSize = size * 0.5f;
    juce::Point<float> topLeft(center.x - halfSize, center.y - halfSize);
    juce::Point<float> topRight(center.x + halfSize, center.y - halfSize);
    juce::Point<float> bottomLeft(center.x - halfSize, center.y + halfSize);
    juce::Point<float> bottomRight(center.x + halfSize, center.y + halfSize);

    g.setColour(juce::Colours::yellow);
    g.drawLine(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y, 2.0f);
    g.drawLine(bottomLeft.x, bottomLeft.y, topRight.x, topRight.y, 2.0f);
}

void BirdsEyeLAF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) {
    float now = juce::Time::getMillisecondCounter() * 0.001;
    float deltaTime = now - lastFrameTime;
    lastFrameTime = now;

    float deltaTimeNorm = deltaTime / 0.01667f;

    juce::Rectangle<float> bounds(x, y, width, height);

    float modifiedValue = distortion.getModifiedCharacterValue();

    // BLINKING
    
    // trigger a blink every blinkInterval
    if (!blinking && now - lastBlinkTime > blinkInterval) {
        blinking = true;
        targetEyelidPosition = 0.0f; // close eyelid
        lastBlinkTime = now;
    }

    juce::Random random;

    // if it reaches the closed position, open it again
    if (blinking && std::abs(eyelidPosition - targetEyelidPosition) < 0.05f) {
        pupilTrackState = random.nextInt(2);
        blinking = false;
    }

    if (!blinking) targetEyelidPosition = 0.5f + modifiedValue / 2.0f;

    eyelidPosition = juce::jmap(0.35f * deltaTimeNorm, eyelidPosition, targetEyelidPosition);

    // PUPIL TRACKING
    if (pupilTrackState == 0) { // Follow mouse
        juce::Point<float> eyeCenter = bounds.getCentre();
        juce::Point<float> mousePos = slider.getLocalPoint(nullptr, juce::Desktop::getInstance().getMainMouseSource().getScreenPosition());
        pupilLookTarget = mousePos - eyeCenter;

        float distance = pupilLookTarget.getDistanceFromOrigin();
        float maxOffset = 9.0f;
        if (distance > maxOffset) pupilLookTarget *= (maxOffset / distance);
    } else {
        pupilLookTarget.setXY(0.0f, 0.0f);
    }

    pupilLook.x = juce::jmap(0.05f * deltaTimeNorm, pupilLook.x, pupilLookTarget.x);
    pupilLook.y = juce::jmap(0.05f * deltaTimeNorm, pupilLook.y, pupilLookTarget.y);

    // DRAWING
    bounds.reduce(3.0f, 3.0f);

    juce::Path eye;

    float eyelidRadians = juce::jmap(juce::jlimit(0.0f, 1.0f, eyelidPosition), 0.1f, juce::MathConstants<float>::pi - 0.1f);
    eye.addArc(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), juce::MathConstants<float>::pi + eyelidRadians, juce::MathConstants<float>::pi - eyelidRadians, true);
    eye.closeSubPath();
    
    g.setColour(juce::Colours::red);
    g.strokePath(eye, juce::PathStrokeType(5, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    float pupilSpacing = 4.0f;
    bounds.reduce(2.0f + pupilSpacing, 2.0f + pupilSpacing);

    float pupilSize    = juce::jmap(sliderPos, 6.0f, bounds.getWidth() / 2.0f);
    float pupilModSize = juce::jmap(modifiedValue, 6.0f, bounds.getWidth() / 2.0f);

    if (pupilModSize < pupilSize) {
        std::swap(pupilSize, pupilModSize);
    }

    juce::Point<float> eyeCenter = bounds.getCentre();

    juce::Rectangle<float> pupilBounds(eyeCenter.x - pupilSize + pupilLook.x, eyeCenter.y - pupilSize + pupilLook.y, pupilSize * 2, pupilSize * 2);
    juce::Rectangle<float> pupilModBounds(eyeCenter.x - pupilModSize + pupilLook.x, eyeCenter.y - pupilModSize + pupilLook.y, pupilModSize * 2, pupilModSize * 2);

    // pupil
    g.reduceClipRegion(eye);
    g.setColour(juce::Colours::yellow);
    g.fillEllipse(pupilModBounds);

    g.setColour(juce::Colours::red);
    g.fillEllipse(pupilBounds);

}

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

    static juce::Font customFont = [] {
        auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::uavosd_ttf,
            BinaryData::uavosd_ttfSize);
        return juce::Font(typeface);
        }();

    g.setFont(customFont.withHeight(11.0f));

    g.setColour(textColor);
    g.drawText("TEST", buttonTop, juce::Justification::centred);
    buttonTop.reduce(1.0f, 1.0f);

    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(buttonTop, 4.0f, 2.1f);
}

constexpr int ditherMap[4][4] = {
    { 1,  8,  2, 10 },
    {12,  4, 14,  6 },
    { 3, 11,  1,  9 },
    {15,  7, 13,  5 }
};

void MixLAF::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, juce::Slider::SliderStyle sliderStyle, juce::Slider& slider) {
    auto bounds = slider.getLocalBounds().toFloat().reduced(5.0f);

    int w = bounds.getWidth() / 5;
    int h = bounds.getHeight() / 5;

    float mix = slider.getValue();

    g.setColour(juce::Colours::red);

    float xPosInterval = 1.0f / (float)w;
    float yPosInterval = xPosInterval / (float)h;

    juce::Point<float> mousePos = slider.getMouseXYRelative().toFloat();

    int barWidth = 2;

    int ditherPixelsWidth = 30;

    int barX = mix * (w - barWidth);

    for (int ix = 0; ix < w; ++ix) {
        float pos = ix * xPosInterval;
        float alpha = 1.0f + (mix - pos) * 2.0f;
        alpha = juce::jlimit(0.0f, 1.0f, alpha);

        if (ix >= barX && ix < barX + barWidth) {
            for (int iy = 0; iy < h; ++iy) {
                juce::Rectangle<float> pixel(bounds.getX() + 1.0f + ix * 5.0f, bounds.getY() + 1.0f + iy * 5.0f, 3.0f, 3.0f);
                g.setColour(juce::Colours::white);
                g.fillRect(pixel);
            }
        } else if (ix < barX) {
            if (ix > barX - ditherPixelsWidth) {
                for (int iy = 0; iy < h; ++iy) {
                    int alpha = juce::jmap(ix, barX - ditherPixelsWidth, barX, 0, 16);
                    int mapValue = ditherMap[ix % 4][iy % 4];

                    if (mapValue >= alpha) {
                        g.setColour(backgroundColor);
                    } else {
                        g.setColour(highlightColor);
                    }

                    juce::Rectangle<float> pixel(bounds.getX() + 1.0f + ix * 5.0f, bounds.getY() + 1.0f + iy * 5.0f, 3.0f, 3.0f);
                    g.fillRect(pixel);
                }
            } else {
                for (int iy = 0; iy < h; ++iy) {
                    juce::Rectangle<float> pixel(bounds.getX() + 1.0f + ix * 5.0f, bounds.getY() + 1.0f + iy * 5.0f, 3.0f, 3.0f);
                    g.setColour(backgroundColor);
                    g.fillRect(pixel);
                }
            }
        }

        /*
        for (int iy = 0; iy < h; ++iy) {

            int mapValue = ditherMap[ix % 4][iy % 4];

            float px = bounds.getX() + 1.0f + ix * 5.0f + 1.5f;
            float py = bounds.getY() + 1.0f + iy * 5.0f + 1.5f;

            float dx = px - mousePos.x;
            float dy = py - mousePos.y;
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist < mouseFadeRadius) {
                float mouseAlpha = 0.0f;
                mouseAlpha = (1.0f - dist / mouseFadeRadius) * mouseFadeDepth;
                mouseAlpha = -mouseAlpha;

                alpha = juce::jlimit(0.0f, 1.0f, alpha + mouseAlpha);
            }

            juce::Rectangle<float> pixel(bounds.getX() + 1.0f + ix * 5.0f,
                bounds.getY() + 1.0f + iy * 5.0f,
                3.0f, 3.0f);

            if (alpha > 0.5f) {
                float scaled = (alpha - 0.5f) * 2.0f * 15.0f;
                if (mapValue <= (int)(scaled + 0.5f))
                    g.setColour(highlightColor);
                else
                    g.setColour(backgroundColor);
                g.fillRect(pixel);
            }
            else if (alpha > 0.0f) {
                float scaled = alpha * 2.0f * 15.0f;
                if (mapValue <= (int)(scaled + 0.5f)) {
                    g.setColour(backgroundColor);
                    g.fillRect(pixel);
                }
            }
            
        }
        */
    }
}

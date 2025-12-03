#include "CustomLafs.h"
#include "Globals.h"

DriveLAF::DriveLAF(DistortionProcessor& dist) : distortion(dist), waveshape{} {
    updateWaveshape();
}

void DriveLAF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) {
    float angle = juce::jmap(sliderPos, rotaryStartAngle, rotaryEndAngle);
    float angleRange = rotaryEndAngle - rotaryStartAngle;

    int centerX = x + width / 2;
    int centerY = y + height / 2;
    float radius = width / 2 - 8;

    float modifiedValue = distortion.getModifiedDriveValue();
    float modifiedAngle = juce::jmap(modifiedValue, rotaryStartAngle, rotaryEndAngle);

    float modStartAngle = (modifiedAngle >= angle) ? angle : modifiedAngle;
    float modEndAngle   = (modifiedAngle >= angle) ? modifiedAngle : angle;

    float arcLength = radius * angleRange;
    int arcSegments = arcLength / 5;
    float arcInterval = angleRange / (float)arcSegments;
    float gap = arcInterval / 5;

    for (int i = 0; i < arcSegments; i++) {
        float startAng = rotaryStartAngle + arcInterval * i;
        float endAng = startAng + arcInterval - gap;

        juce::Path arc;
        arc.addCentredArc(centerX, centerY, radius, radius, 0.0f, startAng, endAng, true);

        if (startAng < modStartAngle) {
            g.setColour(color);
            g.strokePath(arc, juce::PathStrokeType(5, juce::PathStrokeType::mitered, juce::PathStrokeType::butt));
        } else if (startAng < modEndAngle) {
            g.setColour(juce::Colours::yellow);
            g.strokePath(arc, juce::PathStrokeType(5, juce::PathStrokeType::mitered, juce::PathStrokeType::butt));
        }
    }

    updateWaveshape();

    juce::Rectangle<float> bounds = slider.getLocalBounds().toFloat();
    bounds.reduce(bounds.getWidth() * 0.2, bounds.getHeight() * 0.2);
    drawWaveshapePlot(g, bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
}

void DriveLAF::updateWaveshape() {
    std::vector<float> newWaveshape = distortion.getWaveshape(waveshape.size());

    for (size_t i = 0; i < waveshape.size(); i++)
        waveshape[i] = juce::jmap(0.3f, waveshape[i], newWaveshape[i]);
}

void DriveLAF::drawWaveshapePlot(juce::Graphics& g, int x, int y, int width, int height) {
    // Compute geometry
    float halfHeight = y + height * 0.5f;
    float segment = (float)width / (float)waveshape.size();

    // Find max absolute value for scaling
    float maxVal = 1.0f;

    for (float v : waveshape)
        maxVal = std::max(maxVal, std::abs(v));

    float ampScale = (width * 0.5f) / maxVal;

    g.setColour(juce::Colours::darkgrey);
    juce::Path ellipse;
    ellipse.addEllipse(x, y, width, height);
    g.strokePath(ellipse, juce::PathStrokeType(2.0f));

    // Reference line
    float cx = x + width * 0.5f;
    float cy = y + height * 0.5f;
    float r = width * 0.5f;     

    float dx = width;
    float dy = -2.0f * ampScale;
    float len = std::sqrt(dx * dx + dy * dy);

    dx /= len; dy /= len;

    // endpoints touching circle
    g.drawLine(cx - dx * r, cy - dy * r,
        cx + dx * r, cy + dy * r, 2.0f);

    // vertical + horizontal dashed lines
    juce::Path vertical;
    juce::Path dashedVertical;

    juce::Path horizontal;
    juce::Path dashedHorizontal;

    float dashes[] = { 4.0f, 4.0f };
    juce::PathStrokeType stroke(1.0f);

    vertical.startNewSubPath(x + width * 0.5f, y);
    vertical.lineTo(x + width * 0.5f, y + height);
    horizontal.startNewSubPath(x, y + height * 0.5f);
    horizontal.lineTo(x + width, y + height * 0.5f);

    stroke.createDashedStroke(dashedVertical, vertical, dashes, 2);
    stroke.createDashedStroke(dashedHorizontal, horizontal, dashes, 2);

    g.strokePath(dashedVertical, juce::PathStrokeType(1.0f));
    g.strokePath(dashedHorizontal, juce::PathStrokeType(1.0f));

    // Draw waveshape
    juce::Path wave;
    for (int i = 0; i < waveshape.size(); ++i) {
        float px = x + i * segment;
        float py = halfHeight - waveshape[i] * ampScale;
        if (i == 0) wave.startNewSubPath(px, py);
        else        wave.lineTo(px, py);
    }

    g.setColour(color);
    g.strokePath(wave, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void ModSlotLAF::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, juce::Slider::SliderStyle sliderStyle, juce::Slider& slider) {
    juce::Rectangle<float> bounds = slider.getLocalBounds().toFloat().reduced(4.0f);

    float gapSize = 2.0f;
    int segments = 8;

    // Center pivot point
    float centerWidth = 4.0f;

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

    g.setColour(juce::Colours::white);
    g.fillRect(center);

    // Positive values
    for (int i = 0; i < segments; i++) {
        juce::Rectangle<float> segment(centerX + centerWidth + gapSize + (segmentWidth + gapSize) * i, bounds.getY(), segmentWidth, bounds.getHeight());
        float value = juce::jmap((float)i, 0.0f, (float)segments - 1.0f, 0.1f, 0.95f);

        if (value <= slider.getValue()) {
            g.setColour(juce::Colours::yellow);
        } else {
            g.setColour(juce::Colour::fromRGB(127, 127, 0));
        }
        g.fillRect(segment);
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

void BirdsEyeLAF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) {
    float now = juce::Time::getMillisecondCounter() * 0.001;
    float deltaTime = now - lastFrameTime;
    lastFrameTime = now;

    float deltaTimeNorm = deltaTime / 0.01667f;

    juce::Rectangle<float> bounds(x, y, width, height);

    float value = slider.getValue();
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

    if (!blinking) targetEyelidPosition = 0.5f + value / 2.0f;

    eyelidPosition = juce::jmap(0.35f * deltaTimeNorm, eyelidPosition, targetEyelidPosition);

    // PUPIL TRACKING
    if (pupilTrackState == 0) { // Follow mouse
        juce::Point<float> eyeCenter = bounds.getCentre();
        juce::Point<float> mousePos = slider.getLocalPoint(nullptr, juce::Desktop::getInstance().getMainMouseSource().getScreenPosition());
        pupilLookTarget = mousePos - eyeCenter;

        float distance = pupilLookTarget.getDistanceFromOrigin();
        float maxOffset = 9.0f;
        if (distance > maxOffset) pupilLookTarget *= (maxOffset / distance);
    } else { // Looking forward
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
    
    g.setColour(Globals::distortionColor);
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

    g.setColour(Globals::distortionColor);
    g.fillEllipse(pupilBounds);
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

    g.setColour(Globals::distortionColor);

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
    }
}

void SwitchLAF::drawToggleButton(juce::Graphics& g, juce::ToggleButton& toggleButton, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
	targetPos = toggleButton.getToggleState() ? 1.0f : 0.0f;
    
    auto bounds = toggleButton.getLocalBounds().toFloat();

    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(bounds, 5.0f);

    bounds.reduce(5.0f, 5.0f);

    g.setColour(juce::Colours::white);
    bounds = bounds.withTrimmedBottom(bounds.getHeight() / 2.0f);
	bounds.setY(bounds.getY() + (switchPos) * bounds.getHeight());
	g.fillRoundedRectangle(bounds, 3.0f);
}

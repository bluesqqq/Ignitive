#include "CustomLafs.h"

DriveLAF::DriveLAF(DistortionProcessor& dist) : distortion(dist) {}

void DriveLAF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) {
    float angle = juce::jmap(sliderPos, rotaryStartAngle, rotaryEndAngle);
    int centerX = x + width / 2;
    int centerY = y + height / 2;
    float radius = width / 2 - 15;

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

void KnobLAF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) {
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

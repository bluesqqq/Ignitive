#include "CustomLafs.h"

DriveLAF::DriveLAF(DistortionEngine& dist) : distortion(dist) {}

void DriveLAF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) {
    float angle = juce::jmap(sliderPos, rotaryStartAngle, rotaryEndAngle);
    int centerX = x + width / 2;
    int centerY = y + height / 2;
    float radius = width / 2 - 15;

    juce::Path arc;
    arc.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, angle, true);

    g.setColour(juce::Colours::red);
    g.strokePath(arc, juce::PathStrokeType(5, juce::PathStrokeType::mitered, juce::PathStrokeType::butt));

	std::vector<float> waveshape = distortion.getWaveshape();

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
    g.strokePath(path, juce::PathStrokeType(5.0f));
}

void KnobLAF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) {
    g.setColour(juce::Colours::black);
    g.fillEllipse(x, y, width, height);

    g.setColour(juce::Colours::white);
    g.fillEllipse(x + 2, y + 2, width - 4, height - 4);

    g.setColour(juce::Colours::black);

    float angle = juce::jmap(sliderPos, rotaryStartAngle, rotaryEndAngle) - juce::MathConstants<float>::halfPi;
    ;
    float radius = width / 2;
    int centerX = x + width / 2;
    int centerY = y + height / 2;
    g.drawLine(centerX, centerY, centerX + std::cos(angle) * radius, centerY + std::sin(angle) * radius, 3);
}
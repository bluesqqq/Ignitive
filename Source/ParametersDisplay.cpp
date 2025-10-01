#include "ParametersDisplay.h"

ParametersDisplayInterface::ParametersDisplayInterface(const juce::StringArray& parameters) {
    setParameterNames(parameters);
}

void ParametersDisplayInterface::setParameterNames(const juce::StringArray& parameters) {
    int i = 0;
    while (i < parameters.size() && i < 3) {
        paramNames.add(parameters[i]);
        ++i;
    }
    while (i < 3) {
        paramNames.add("---");
        ++i;
    }
}

const juce::StringArray& ParametersDisplayInterface::getParamNames() { return paramNames; }

juce::String ParametersDisplayInterface::getValue(int index) {
    if (index < 0 || index >= valueGetters.size()) return "---";
    return valueGetters[index]();
}



void ParametersDisplay::setInterface(ParametersDisplayInterface* newInterface) {
    if (newInterface == nullptr) return;
    currentInterface = newInterface;
    targetParamNames = currentInterface->getParamNames();
}

void ParametersDisplay::showValue(int index) {
    if (index < 0 || index >= showValues.size()) return;
    showValues[index] = true;
}

void ParametersDisplay::hideValue(int index) {
    if (index < 0 || index >= showValues.size()) return;
    showValues[index] = false;
}

void ParametersDisplay::paint(juce::Graphics& g) {
    font.setHeight(modParamNamesFontSize);
    g.setFont(font);
    g.setColour(juce::Colours::yellow);

    if (currentInterface == nullptr) return;

    auto bounds = getLocalBounds().toFloat();

    float sectionWidth = bounds.getWidth() / 3.0f;

    for (int i = 0; i < paramNames.size(); ++i) {
        juce::String displayString;
        if (!showValues[i]) {
            juce::String& target = targetParamNames.getReference(i);

            juce::String& current = paramNames.getReference(i);

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

            displayString = current;
        } else {
            displayString = currentInterface->getValue(i);
        }

        juce::Rectangle<float> sectionRect(bounds.getX() + i * sectionWidth, bounds.getY(), sectionWidth, bounds.getHeight());

        g.drawText(displayString, sectionRect.toNearestInt(), juce::Justification::centred);
    }
}
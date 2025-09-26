#pragma once

#include <JuceHeader.h>

class ParametersDisplay : public juce::Component {
	private:
		juce::StringArray paramNames{ "---", "---", "---" };
		juce::StringArray paramNamesEnvelope{ "Attack", "Decay", "Gate" };
		juce::StringArray paramNamesLFO{ "Speed", "---", "---" };
		float modParamNamesFontSize = 14.0f;

        juce::String symbols = ".#:_+=@!";
        juce::Random r;

        bool showingEnvelope = false;

        juce::Font font;

	public:
        ParametersDisplay(const juce::Font& font) : font(font) {}

        void setState(bool showEnv) {
            showingEnvelope = showEnv;
        }

        void paint(juce::Graphics& g) override {
            font.setHeight(modParamNamesFontSize);
            g.setFont(font);
            g.setColour(juce::Colours::yellow);

            const juce::StringArray& usingArray = showingEnvelope ? paramNamesEnvelope : paramNamesLFO;
            auto bounds = getLocalBounds().toFloat();

            float sectionWidth = bounds.getWidth() / 3.0f;

            for (int i = 0; i < paramNames.size(); ++i) {
                juce::String target = usingArray[i];

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

                juce::Rectangle<float> sectionRect(bounds.getX() + i * sectionWidth, bounds.getY(), sectionWidth, bounds.getHeight());

                g.drawText(current, sectionRect.toNearestInt(), juce::Justification::centred);
            }
        }
};
#pragma once

#include <JuceHeader.h>

/// <summary>
/// An Interface for ParametersDisplay to get parameter names and values from.
/// </summary>
class ParametersDisplayInterface {
    /*
        This class is a little overkill, and mostly just solving a problem I created, but
        i find it satisfying to implement these kind of solutions.
    */
    protected:
        juce::StringArray paramNames;

        std::vector<std::function<juce::String()>> valueGetters;

    public:
        ParametersDisplayInterface(const juce::StringArray& parameters);

        void setParameterNames(const juce::StringArray& parameters);

        const juce::StringArray& getParamNames();
        juce::String getValue(int index);
};

class ParametersDisplay : public juce::Component {
	private:
		ParametersDisplayInterface* currentInterface = nullptr;

		juce::StringArray paramNames{ "---", "---", "---" };
        juce::StringArray targetParamNames{ "---", "---", "---" };

		float modParamNamesFontSize = 14.0f;

		std::vector<bool> showValues{ false, false, false };

        juce::String symbols = ".#:_+=@!";
        juce::Random r;

        juce::Font font;

	public:
        ParametersDisplay(const juce::Font& font) : font(font) {}

        void setInterface(ParametersDisplayInterface* newInterface);
        void showValue(int index);

        void hideValue(int index);

        void paint(juce::Graphics& g) override;
};
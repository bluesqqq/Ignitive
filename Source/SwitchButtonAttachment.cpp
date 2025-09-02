#include "SwitchButtonAttachment.h"

void SwitchButtonAttachment::parameterChanged(const juce::String& id, float newValue) {
    if (id == parameterID) {
        if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(id))) {
            int index = choiceParam->getIndex();
            juce::Component::SafePointer<SwitchButton> safeButton(&button);

            juce::MessageManager::callAsync([safeButton, index] {
                if (safeButton != nullptr)
                    safeButton->setIndex(index);
                });
        }
    }
}

SwitchButtonAttachment::SwitchButtonAttachment(juce::AudioProcessorValueTreeState& _apvts, const juce::String& _parameterID, SwitchButton& _button)
    : apvts(_apvts), parameterID(_parameterID), button(_button) {
    apvts.addParameterListener(parameterID, this);

    button.onIndexChange = [this](int newIndex) {
        if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(parameterID))) {
            choiceParam->beginChangeGesture();
            choiceParam->setValueNotifyingHost(choiceParam->getNormalisableRange().convertTo0to1(newIndex));
            choiceParam->endChangeGesture();
        }
        };

    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(parameterID))) {
        button.setIndex(choiceParam->getIndex());
    }
}

SwitchButtonAttachment::~SwitchButtonAttachment() { apvts.removeParameterListener(parameterID, this); }

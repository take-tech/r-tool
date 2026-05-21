#include "Parameters.h"

namespace rtool::param
{
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { swap, 1 },
            "Swap",
            juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f },
            0.0f));

        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { volumeDb, 1 },
            "Volume",
            juce::NormalisableRange<float> { -100.0f, 18.1f, 0.01f },
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("dB")));

        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { gainDb, 1 },
            "Gain",
            juce::NormalisableRange<float> { -36.0f, 36.0f, 0.01f },
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("dB")));

        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { pan, 1 },
            "Pan",
            juce::NormalisableRange<float> { -1.0f, 1.0f, 0.01f },
            0.0f));

        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { width, 1 },
            "Width",
            juce::NormalisableRange<float> { 0.0f, 2.0f, 0.01f },
            1.0f));

        parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID { theme, 1 },
            "Theme",
            juce::StringArray { "Ember", "Amber", "Ruby", "Graphite", "Blush" },
            0));

        return { parameters.begin(), parameters.end() };
    }
}

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace rtool::param
{
    static constexpr auto swap        = "swap";
    static constexpr auto volumeDb    = "volumeDb";
    static constexpr auto gainDb      = "gainDb";
    static constexpr auto pan         = "pan";
    static constexpr auto width       = "width";
    static constexpr auto theme       = "theme";

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
}

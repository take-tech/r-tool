#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace rtool
{
namespace dsp
{
    class ToolProcessor
    {
    public:
        void prepare(double sampleRate, int samplesPerBlock, int numChannels);
        void reset();
        void setParameters(
            float swap,
            float volumeDb,
            float gainDb,
            float pan,
            float width);
        void processBlock(juce::AudioBuffer<float>& buffer);

    private:
        double sampleRate { 44100.0 };
        int numChannels { 0 };

        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gainSmoothed;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> panLeftSmoothed;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> panRightSmoothed;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> widthSmoothed;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> swapSmoothed;

        static float computeGainLinear(float volumeDb, float gainDb) noexcept;
    };
}
}

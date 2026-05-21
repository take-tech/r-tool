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
            bool invertLeft,
            bool swapLR,
            bool invertRight,
            float volumeDb,
            float gainDb,
            float pan,
            float width);
        void processBlock(juce::AudioBuffer<float>& buffer);

    private:
        double sampleRate { 44100.0 };
        int numChannels { 0 };

        bool paramInvertLeft  { false };
        bool paramSwapLR      { false };
        bool paramInvertRight { false };
        float paramVolumeDb   { 0.0f };
        float paramGainDb     { 0.0f };
        float paramPan        { 0.0f };
        float paramWidth      { 1.0f };

        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gainSmoothed;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> panLeftSmoothed;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> panRightSmoothed;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> widthSmoothed;

        static float computeGainLinear(float volumeDb, float gainDb) noexcept;
    };
}
}

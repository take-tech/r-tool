#include "ToolProcessor.h"

#include <cmath>

namespace rtool
{
namespace dsp
{
    void ToolProcessor::prepare(double sr, int, int numChans)
    {
        sampleRate  = sr > 0.0 ? sr : 44100.0;
        numChannels = numChans;

        gainSmoothed.reset(sampleRate, 0.02);
        gainSmoothed.setCurrentAndTargetValue(1.0f);

        panLeftSmoothed.reset(sampleRate, 0.02);
        panLeftSmoothed.setCurrentAndTargetValue(1.0f);

        panRightSmoothed.reset(sampleRate, 0.02);
        panRightSmoothed.setCurrentAndTargetValue(1.0f);

        widthSmoothed.reset(sampleRate, 0.02);
        widthSmoothed.setCurrentAndTargetValue(1.0f);

        swapSmoothed.reset(sampleRate, 0.02);
        swapSmoothed.setCurrentAndTargetValue(0.0f);
    }

    void ToolProcessor::reset()
    {
        gainSmoothed.reset(sampleRate, 0.02);
        panLeftSmoothed.reset(sampleRate, 0.02);
        panRightSmoothed.reset(sampleRate, 0.02);
        widthSmoothed.reset(sampleRate, 0.02);
        swapSmoothed.reset(sampleRate, 0.02);
    }

    void ToolProcessor::setParameters(
        float swap,
        float volumeDb,
        float gainDb,
        float pan,
        float width)
    {
        gainSmoothed.setTargetValue(computeGainLinear(volumeDb, gainDb));

        const auto panNorm = (pan + 1.0f) * 0.5f;
        const auto sq2     = std::sqrt(2.0f);
        const auto halfPi  = juce::MathConstants<float>::halfPi;
        panLeftSmoothed.setTargetValue(std::cos(panNorm * halfPi) * sq2);
        panRightSmoothed.setTargetValue(std::sin(panNorm * halfPi) * sq2);

        widthSmoothed.setTargetValue(width);
        swapSmoothed.setTargetValue(juce::jlimit(0.0f, 1.0f, swap));
    }

    void ToolProcessor::processBlock(juce::AudioBuffer<float>& buffer)
    {
        const auto numChans   = buffer.getNumChannels();
        const auto numSamples = buffer.getNumSamples();

        if (numChans == 0 || numSamples == 0)
            return;

        if (numChans == 1)
        {
            auto* data = buffer.getWritePointer(0);
            for (int i = 0; i < numSamples; ++i)
            {
                const auto gain = gainSmoothed.getNextValue();
                panLeftSmoothed.getNextValue();
                panRightSmoothed.getNextValue();
                widthSmoothed.getNextValue();
                swapSmoothed.getNextValue();

                data[i] = data[i] * gain;
            }
            return;
        }

        auto* leftData  = buffer.getWritePointer(0);
        auto* rightData = buffer.getWritePointer(1);

        for (int i = 0; i < numSamples; ++i)
        {
            const auto gain  = gainSmoothed.getNextValue();
            const auto panL  = panLeftSmoothed.getNextValue();
            const auto panR  = panRightSmoothed.getNextValue();
            const auto w     = widthSmoothed.getNextValue();
            const auto s     = swapSmoothed.getNextValue();

            float left  = leftData[i]  * gain;
            float right = rightData[i] * gain;

            // Continuous L/R crossfade: 0=normal, 0.5=mono, 1.0=swapped
            const auto newLeft  = left  * (1.0f - s) + right * s;
            const auto newRight = right * (1.0f - s) + left  * s;
            left  = newLeft;
            right = newRight;

            left  *= panL;
            right *= panR;

            // Mid/Side width processing: width=1.0 is unity
            const auto mid  = (left + right) * 0.5f;
            const auto side = (left - right) * 0.5f * w;
            leftData[i]  = mid + side;
            rightData[i] = mid - side;
        }
    }

    float ToolProcessor::computeGainLinear(float volumeDb, float gainDb) noexcept
    {
        if (volumeDb <= -100.0f)
            return 0.0f;

        return juce::Decibels::decibelsToGain(volumeDb + gainDb);
    }
}
}

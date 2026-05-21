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
    }

    void ToolProcessor::reset()
    {
        gainSmoothed.reset(sampleRate, 0.02);
        panLeftSmoothed.reset(sampleRate, 0.02);
        panRightSmoothed.reset(sampleRate, 0.02);
        widthSmoothed.reset(sampleRate, 0.02);
    }

    void ToolProcessor::setParameters(
        bool invertLeft,
        bool swapLR,
        bool invertRight,
        float volumeDb,
        float gainDb,
        float pan,
        float width)
    {
        paramInvertLeft  = invertLeft;
        paramSwapLR      = swapLR;
        paramInvertRight = invertRight;
        paramVolumeDb    = volumeDb;
        paramGainDb      = gainDb;
        paramPan         = pan;
        paramWidth       = width;

        gainSmoothed.setTargetValue(computeGainLinear(volumeDb, gainDb));

        // Equal-power pan: pre-compute L/R gains from pan [-1, 1]
        // At centre (pan=0): leftGain=rightGain=1.0 (unity, no change)
        const auto panNorm  = (pan + 1.0f) * 0.5f;
        const auto sq2      = std::sqrt(2.0f);
        const auto halfPi   = juce::MathConstants<float>::halfPi;
        panLeftSmoothed.setTargetValue(std::cos(panNorm * halfPi) * sq2);
        panRightSmoothed.setTargetValue(std::sin(panNorm * halfPi) * sq2);

        widthSmoothed.setTargetValue(width);
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

                float v = data[i] * gain;
                if (paramInvertLeft)  v = -v;
                if (paramInvertRight) v = -v;
                data[i] = v;
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

            float left  = leftData[i]  * gain;
            float right = rightData[i] * gain;

            if (paramInvertLeft)  left  = -left;
            if (paramInvertRight) right = -right;

            if (paramSwapLR) std::swap(left, right);

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

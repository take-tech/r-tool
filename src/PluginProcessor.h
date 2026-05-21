#pragma once

#include <atomic>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Parameters.h"
#include "dsp/ToolProcessor.h"

class RToolAudioProcessor final : public juce::AudioProcessor
{
public:
    RToolAudioProcessor();
    ~RToolAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getValueTreeState() noexcept { return apvts; }

    float getLeftMeterLevel()  const noexcept;
    float getRightMeterLevel() const noexcept;

private:
    void updateOutputMeter(const juce::AudioBuffer<float>& buffer) noexcept;

    juce::AudioProcessorValueTreeState apvts;
    rtool::dsp::ToolProcessor toolProcessor;

    std::atomic<float> leftMeterLevel  { 0.0f };
    std::atomic<float> rightMeterLevel { 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RToolAudioProcessor)
};

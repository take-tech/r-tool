#include "PluginProcessor.h"

#include "PluginEditor.h"
#include "UserSettings.h"

RToolAudioProcessor::RToolAudioProcessor()
    : AudioProcessor(BusesProperties()
          .withInput("Input",   juce::AudioChannelSet::stereo(), true)
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", rtool::param::createParameterLayout())
{
    if (const auto themeIndex = rtool::settings::loadDefaultThemeIndex())
    {
        if (auto* parameter = apvts.getParameter(rtool::param::theme))
            parameter->setValueNotifyingHost(parameter->convertTo0to1(static_cast<float>(*themeIndex)));
    }
}

void RToolAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    toolProcessor.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());

    toolProcessor.setParameters(
        *apvts.getRawParameterValue(rtool::param::invertLeft)  > 0.5f,
        *apvts.getRawParameterValue(rtool::param::swapLR)      > 0.5f,
        *apvts.getRawParameterValue(rtool::param::invertRight) > 0.5f,
        *apvts.getRawParameterValue(rtool::param::volumeDb),
        *apvts.getRawParameterValue(rtool::param::gainDb),
        *apvts.getRawParameterValue(rtool::param::pan),
        *apvts.getRawParameterValue(rtool::param::width));

    leftMeterLevel.store(0.0f,  std::memory_order_relaxed);
    rightMeterLevel.store(0.0f, std::memory_order_relaxed);
}

void RToolAudioProcessor::releaseResources()
{
    toolProcessor.reset();
}

bool RToolAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& mainInput  = layouts.getMainInputChannelSet();
    const auto& mainOutput = layouts.getMainOutputChannelSet();

    if (mainInput != mainOutput)
        return false;

    return mainOutput == juce::AudioChannelSet::mono()
        || mainOutput == juce::AudioChannelSet::stereo();
}

void RToolAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const auto totalInputChannels  = getTotalNumInputChannels();
    const auto totalOutputChannels = getTotalNumOutputChannels();

    for (auto ch = totalInputChannels; ch < totalOutputChannels; ++ch)
        buffer.clear(ch, 0, buffer.getNumSamples());

    toolProcessor.setParameters(
        *apvts.getRawParameterValue(rtool::param::invertLeft)  > 0.5f,
        *apvts.getRawParameterValue(rtool::param::swapLR)      > 0.5f,
        *apvts.getRawParameterValue(rtool::param::invertRight) > 0.5f,
        *apvts.getRawParameterValue(rtool::param::volumeDb),
        *apvts.getRawParameterValue(rtool::param::gainDb),
        *apvts.getRawParameterValue(rtool::param::pan),
        *apvts.getRawParameterValue(rtool::param::width));

    toolProcessor.processBlock(buffer);
    updateOutputMeter(buffer);
}

juce::AudioProcessorEditor* RToolAudioProcessor::createEditor()
{
    return new RToolAudioProcessorEditor(*this);
}

bool RToolAudioProcessor::hasEditor() const { return true; }

const juce::String RToolAudioProcessor::getName() const { return JucePlugin_Name; }

bool RToolAudioProcessor::acceptsMidi()  const { return false; }
bool RToolAudioProcessor::producesMidi() const { return false; }
bool RToolAudioProcessor::isMidiEffect() const { return false; }
double RToolAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int RToolAudioProcessor::getNumPrograms()                        { return 1; }
int RToolAudioProcessor::getCurrentProgram()                     { return 0; }
void RToolAudioProcessor::setCurrentProgram(int)                 {}
const juce::String RToolAudioProcessor::getProgramName(int)      { return {}; }
void RToolAudioProcessor::changeProgramName(int, const juce::String&) {}

void RToolAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState(); state.isValid())
        if (auto xml = state.createXml())
            copyXmlToBinary(*xml, destData);
}

void RToolAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

float RToolAudioProcessor::getLeftMeterLevel() const noexcept
{
    return leftMeterLevel.load(std::memory_order_relaxed);
}

float RToolAudioProcessor::getRightMeterLevel() const noexcept
{
    return rightMeterLevel.load(std::memory_order_relaxed);
}

void RToolAudioProcessor::updateOutputMeter(const juce::AudioBuffer<float>& buffer) noexcept
{
    const auto numSamples = buffer.getNumSamples();

    if (buffer.getNumChannels() == 0 || numSamples == 0)
    {
        leftMeterLevel.store(0.0f,  std::memory_order_relaxed);
        rightMeterLevel.store(0.0f, std::memory_order_relaxed);
        return;
    }

    const auto leftPeak  = buffer.getMagnitude(0, 0, numSamples);
    const auto rightPeak = buffer.getNumChannels() > 1
        ? buffer.getMagnitude(1, 0, numSamples)
        : leftPeak;

    leftMeterLevel.store(leftPeak,  std::memory_order_relaxed);
    rightMeterLevel.store(rightPeak, std::memory_order_relaxed);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RToolAudioProcessor();
}

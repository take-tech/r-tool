#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <ranze_plugin_ui/Theme.h>

#include "PluginProcessor.h"
#include "gui/MeterComponent.h"

class RToolAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                        private juce::AudioProcessorValueTreeState::Listener,
                                        private juce::AsyncUpdater,
                                        private juce::Timer
{
public:
    explicit RToolAudioProcessorEditor(RToolAudioProcessor&);
    ~RToolAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment  = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment  = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    class ThemeIconButton final : public juce::Button
    {
    public:
        ThemeIconButton();
        void setTheme(ranze::ui::Theme newTheme);
        void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    private:
        ranze::ui::Theme theme { ranze::ui::getDefaultTheme() };
    };

    void timerCallback() override;
    void configureSliderText();
    void configureThemeButton();
    void showThemeMenu();
    void setThemeIndex(int themeIndex);
    void applyTheme();
    int  getCurrentThemeIndex() const;
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void handleAsyncUpdate() override;

    RToolAudioProcessor& audioProcessor;
    ranze::ui::Theme theme { ranze::ui::getDefaultTheme() };

    // Top-row toggle buttons
    juce::TextButton invertLeftButton  { "L-" };
    juce::TextButton swapLRButton      { "Swap L/R" };
    juce::TextButton invertRightButton { "R-" };
    std::unique_ptr<ButtonAttachment> invertLeftAttachment;
    std::unique_ptr<ButtonAttachment> swapLRAttachment;
    std::unique_ptr<ButtonAttachment> invertRightAttachment;

    // Knobs
    juce::Slider volumeSlider;
    juce::Slider gainSlider;
    juce::Slider panSlider;
    juce::Slider widthSlider;

    juce::Label volumeLabel;
    juce::Label gainLabel;
    juce::Label panLabel;
    juce::Label widthLabel;

    std::unique_ptr<SliderAttachment> volumeAttachment;
    std::unique_ptr<SliderAttachment> gainAttachment;
    std::unique_ptr<SliderAttachment> panAttachment;
    std::unique_ptr<SliderAttachment> widthAttachment;

    // Meter
    rtool::gui::MeterComponent meterComponent;

    // Theme selector (hidden combo box driven by the theme button)
    ThemeIconButton themeButton;
    juce::ComboBox themeSelector;
    std::unique_ptr<ComboBoxAttachment> themeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RToolAudioProcessorEditor)
};

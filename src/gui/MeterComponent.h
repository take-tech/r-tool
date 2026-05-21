#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <ranze_plugin_ui/Theme.h>

namespace rtool
{
namespace gui
{
    class MeterComponent final : public juce::Component
    {
    public:
        MeterComponent() = default;

        void setLevels(float left, float right);
        void setTheme(ranze::ui::Theme newTheme);

        void paint(juce::Graphics& g) override;

    private:
        void drawScaleMarks(juce::Graphics& g,
                            juce::Rectangle<float> scaleArea,
                            juce::Rectangle<float> meterBounds) const;

        float leftLevel  { 0.0f };
        float rightLevel { 0.0f };
        ranze::ui::Theme theme { ranze::ui::getDefaultTheme() };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeterComponent)
    };
}
}

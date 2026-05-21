#include "MeterComponent.h"

namespace rtool
{
namespace gui
{
    void MeterComponent::setLevels(float left, float right)
    {
        leftLevel  = left;
        rightLevel = right;
    }

    void MeterComponent::setTheme(ranze::ui::Theme newTheme)
    {
        theme = newTheme;
        repaint();
    }

    void MeterComponent::paint(juce::Graphics& g)
    {
        const auto bounds   = getLocalBounds().toFloat().reduced(0.0f, 4.0f);
        const auto scaleW   = 30.0f;
        const auto meterW   = 16.0f;
        const auto gapSM    =  2.0f;  // scale → L meter
        const auto gapMM    =  4.0f;  // L meter → R meter
        const auto totalW   = scaleW + gapSM + meterW + gapMM + meterW;
        const auto area     = bounds.withSizeKeepingCentre(totalW, bounds.getHeight());

        const auto scaleArea  = area.withWidth(scaleW);
        const auto lMeterArea = area.withX(area.getX() + scaleW + gapSM).withWidth(meterW);
        const auto rMeterArea = lMeterArea.withX(lMeterArea.getRight() + gapMM);

        ranze::ui::drawVerticalPeakMeter(g, lMeterArea, theme, leftLevel,  "L");
        ranze::ui::drawVerticalPeakMeter(g, rMeterArea, theme, rightLevel, "R");
        drawScaleMarks(g, scaleArea, lMeterArea);
    }

    void MeterComponent::drawScaleMarks(juce::Graphics& g,
                                        juce::Rectangle<float> scaleArea,
                                        juce::Rectangle<float> meterBounds) const
    {
        // fillBounds inside drawVerticalPeakMeter is meterBounds.reduced(3.0f)
        const auto fill   = meterBounds.reduced(3.0f);
        const auto bottom = fill.getBottom();
        const auto height = fill.getHeight();

        static constexpr float marks[] = { 0.0f, -12.0f, -24.0f, -48.0f };

        // Separate text area (left) from tick area (right 6px)
        const auto tickX1   = scaleArea.getRight() - 5.0f;
        const auto tickX2   = scaleArea.getRight();
        const auto textArea = scaleArea.withRight(tickX1 - 2.0f);

        g.setFont(juce::FontOptions(11.0f));

        for (const auto db : marks)
        {
            const auto norm = (db + 60.0f) / 60.0f;
            const auto y    = bottom - height * norm;
            const auto yInt = static_cast<int>(std::round(y));

            g.setColour(theme.mutedText.withAlpha(0.6f));
            g.drawHorizontalLine(yInt, tickX1, tickX2);

            const auto label = db == 0.0f ? juce::String("0") : juce::String(static_cast<int>(db));
            g.setColour(theme.mutedText.withAlpha(0.75f));
            g.drawText(label,
                       textArea.withY(y - 6.0f).withHeight(12.0f),
                       juce::Justification::centredRight,
                       false);
        }
    }
}
}

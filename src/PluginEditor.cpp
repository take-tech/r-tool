#include "PluginEditor.h"

#include "Parameters.h"
#include "UserSettings.h"

#include <cmath>

// ---------------------------------------------------------------------------
// Editor
// ---------------------------------------------------------------------------

RToolAudioProcessorEditor::RToolAudioProcessorEditor(RToolAudioProcessor& processorRef)
    : AudioProcessorEditor(&processorRef),
      audioProcessor(processorRef)
{
    setSize(360, 316);

    // --- Toggle buttons ---
    for (auto* btn : { &invertLeftButton, &swapLRButton, &invertRightButton })
    {
        btn->setClickingTogglesState(true);
        addAndMakeVisible(btn);
    }

    invertLeftAttachment  = std::make_unique<ButtonAttachment>(audioProcessor.getValueTreeState(), rtool::param::invertLeft,  invertLeftButton);
    swapLRAttachment      = std::make_unique<ButtonAttachment>(audioProcessor.getValueTreeState(), rtool::param::swapLR,      swapLRButton);
    invertRightAttachment = std::make_unique<ButtonAttachment>(audioProcessor.getValueTreeState(), rtool::param::invertRight, invertRightButton);

    // --- Knobs ---
    auto setupKnob = [this](juce::Slider& s, juce::Label& lbl, const juce::String& text)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 84, 20);
        addAndMakeVisible(s);

        lbl.setText(text, juce::dontSendNotification);
        lbl.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(lbl);
    };

    setupKnob(volumeSlider, volumeLabel, "Volume");
    setupKnob(gainSlider,   gainLabel,   "Gain");
    setupKnob(panSlider,    panLabel,    "Pan");
    setupKnob(widthSlider,  widthLabel,  "Width");

    configureSliderText();

    auto& vts = audioProcessor.getValueTreeState();
    volumeAttachment = std::make_unique<SliderAttachment>(vts, rtool::param::volumeDb, volumeSlider);
    gainAttachment   = std::make_unique<SliderAttachment>(vts, rtool::param::gainDb,   gainSlider);
    panAttachment    = std::make_unique<SliderAttachment>(vts, rtool::param::pan,       panSlider);
    widthAttachment  = std::make_unique<SliderAttachment>(vts, rtool::param::width,     widthSlider);

    // --- Meter ---
    addAndMakeVisible(meterComponent);

    // --- Theme ---
    configureThemeButton();
    vts.addParameterListener(rtool::param::theme, this);

    applyTheme();
    startTimerHz(30);
}

RToolAudioProcessorEditor::~RToolAudioProcessorEditor()
{
    stopTimer();
    audioProcessor.getValueTreeState().removeParameterListener(rtool::param::theme, this);
}

void RToolAudioProcessorEditor::paint(juce::Graphics& g)
{
    ranze::ui::drawPluginPanel(g, getLocalBounds(), theme);
    ranze::ui::drawPluginTitle(g, { 28, 16, 140, 28 }, theme, "R-Tool");
}

void RToolAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(16);

    // --- Header: title placeholder + theme button ---
    auto header = area.removeFromTop(28);
    themeButton.setBounds(header.removeFromRight(28).withHeight(28));

    // --- Right: meter strip ---
    auto meterArea = area.removeFromRight(82);
    meterComponent.setBounds(meterArea);
    area.removeFromRight(8); // gap

    // --- Button row ---
    auto buttonRow = area.removeFromTop(32);
    area.removeFromTop(8);

    const int totalBtnW = buttonRow.getWidth();
    const int btn1W = 60, btn2W = totalBtnW - 120, btn3W = 60;
    const int gap   = (totalBtnW - btn1W - btn2W - btn3W) / 2;

    invertLeftButton.setBounds(buttonRow.removeFromLeft(btn1W));
    buttonRow.removeFromLeft(gap);
    swapLRButton.setBounds(buttonRow.removeFromLeft(btn2W));
    buttonRow.removeFromLeft(gap);
    invertRightButton.setBounds(buttonRow);

    // --- Knob rows: 2 columns each ---
    auto row1 = area.removeFromTop(105);
    area.removeFromTop(6);
    auto row2 = area;

    const int colW = row1.getWidth() / 2;

    auto placeKnob = [&](juce::Rectangle<int> cell, juce::Slider& s, juce::Label& lbl)
    {
        auto labelRect = cell.removeFromTop(18);
        lbl.setBounds(labelRect);
        s.setBounds(cell);
    };

    placeKnob(row1.removeFromLeft(colW), volumeSlider, volumeLabel);
    placeKnob(row1,                      gainSlider,   gainLabel);
    placeKnob(row2.removeFromLeft(colW), panSlider,    panLabel);
    placeKnob(row2,                      widthSlider,  widthLabel);
}

void RToolAudioProcessorEditor::timerCallback()
{
    meterComponent.setLevels(
        audioProcessor.getLeftMeterLevel(),
        audioProcessor.getRightMeterLevel());
    meterComponent.repaint();
}

void RToolAudioProcessorEditor::configureSliderText()
{
    volumeSlider.textFromValueFunction = [](double v) -> juce::String
    {
        if (v <= -100.0)
            return "-∞ dB";
        return juce::String(v, 1) + " dB";
    };
    volumeSlider.valueFromTextFunction = [](const juce::String& t) -> double
    {
        if (t.containsChar(0x221e) || t.containsIgnoreCase("inf"))
            return -100.0;
        return t.getDoubleValue();
    };

    gainSlider.textFromValueFunction = [](double v) -> juce::String
    {
        if (v > 0.0)
            return "+" + juce::String(v, 1) + " dB";
        return juce::String(v, 1) + " dB";
    };

    panSlider.textFromValueFunction = [](double v) -> juce::String
    {
        if (std::abs(v) < 0.005)
            return "C";
        const int pct = static_cast<int>(std::round(std::abs(v) * 100.0));
        return v < 0.0 ? "L" + juce::String(pct) : "R" + juce::String(pct);
    };
    panSlider.valueFromTextFunction = [](const juce::String& t) -> double
    {
        if (t.equalsIgnoreCase("C")) return 0.0;
        if (t.startsWithIgnoreCase("L")) return -t.substring(1).getDoubleValue() / 100.0;
        if (t.startsWithIgnoreCase("R")) return  t.substring(1).getDoubleValue() / 100.0;
        return t.getDoubleValue();
    };

    widthSlider.textFromValueFunction = [](double v) -> juce::String
    {
        return juce::String(static_cast<int>(std::round(v * 100.0))) + " %";
    };
    widthSlider.valueFromTextFunction = [](const juce::String& t) -> double
    {
        return t.getDoubleValue() / 100.0;
    };
}

void RToolAudioProcessorEditor::configureThemeButton()
{
    themeSelector.addItem("Ember",    1);
    themeSelector.addItem("Amber",    2);
    themeSelector.addItem("Ruby",     3);
    themeSelector.addItem("Graphite", 4);
    themeSelector.addItem("Blush",    5);
    themeSelector.onChange = [this] { applyTheme(); };
    addChildComponent(themeSelector);

    themeAttachment = std::make_unique<ComboBoxAttachment>(
        audioProcessor.getValueTreeState(), rtool::param::theme, themeSelector);

    themeButton.onClick = [this] { showThemeMenu(); };
    addAndMakeVisible(themeButton);
}

void RToolAudioProcessorEditor::showThemeMenu()
{
    juce::PopupMenu menu;
    const auto selected = getCurrentThemeIndex();

    menu.addItem(1, "Ember",    true, selected == 0);
    menu.addItem(2, "Amber",    true, selected == 1);
    menu.addItem(3, "Ruby",     true, selected == 2);
    menu.addItem(4, "Graphite", true, selected == 3);
    menu.addItem(5, "Blush",    true, selected == 4);

    menu.showMenuAsync(
        juce::PopupMenu::Options().withTargetComponent(themeButton),
        [this](int result)
        {
            if (result > 0)
                setThemeIndex(result - 1);
        });
}

void RToolAudioProcessorEditor::setThemeIndex(int themeIndex)
{
    rtool::settings::saveDefaultThemeIndex(themeIndex);
    themeSelector.setSelectedId(themeIndex + 1, juce::sendNotificationSync);
}

void RToolAudioProcessorEditor::applyTheme()
{
    const auto idx = getCurrentThemeIndex();
    const auto tid = ranze::ui::getThemeIdForIndex(idx);
    theme = ranze::ui::getThemeForIndex(idx);

    // Toggle buttons
    for (auto* btn : { &invertLeftButton, &swapLRButton, &invertRightButton })
    {
        btn->setColour(juce::TextButton::buttonColourId,   theme.control);
        btn->setColour(juce::TextButton::buttonOnColourId, theme.accent);
        btn->setColour(juce::TextButton::textColourOffId,  theme.text);
        btn->setColour(juce::TextButton::textColourOnId,   theme.background);
    }

    // Knobs and labels
    for (auto* s : { &volumeSlider, &gainSlider, &panSlider, &widthSlider })
        ranze::ui::applyRotarySliderTheme(*s, theme, tid);

    for (auto* lbl : { &volumeLabel, &gainLabel, &panLabel, &widthLabel })
        ranze::ui::applyLabelTheme(*lbl, theme);

    meterComponent.setTheme(theme);
    themeButton.setTheme(theme);

    repaint();
}

int RToolAudioProcessorEditor::getCurrentThemeIndex() const
{
    const auto selectedId = themeSelector.getSelectedId();
    if (selectedId > 0)
        return juce::jlimit(0, 4, selectedId - 1);

    if (auto* v = audioProcessor.getValueTreeState().getRawParameterValue(rtool::param::theme))
        return juce::jlimit(0, 4, static_cast<int>(std::round(v->load())));

    return 0;
}

void RToolAudioProcessorEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    juce::ignoreUnused(newValue);
    if (parameterID == rtool::param::theme)
        triggerAsyncUpdate();
}

void RToolAudioProcessorEditor::handleAsyncUpdate()
{
    applyTheme();
}

// ---------------------------------------------------------------------------
// ThemeIconButton
// ---------------------------------------------------------------------------

RToolAudioProcessorEditor::ThemeIconButton::ThemeIconButton()
    : juce::Button("Theme")
{
    setTitle("Theme");
}

void RToolAudioProcessorEditor::ThemeIconButton::setTheme(ranze::ui::Theme newTheme)
{
    theme = newTheme;
    repaint();
}

void RToolAudioProcessorEditor::ThemeIconButton::paintButton(
    juce::Graphics& g,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    const auto bounds = getLocalBounds().toFloat().reduced(1.0f);
    const auto alpha  = shouldDrawButtonAsDown ? 0.95f : (shouldDrawButtonAsHighlighted ? 0.85f : 0.72f);

    g.setColour(theme.control.withAlpha(alpha));
    g.fillRoundedRectangle(bounds, 6.0f);

    g.setColour(theme.accent.withAlpha(0.8f));
    g.drawRoundedRectangle(bounds, 6.0f, 1.2f);

    const auto dotArea = bounds.reduced(6.0f, 5.0f);
    const auto r = 3.0f;
    g.setColour(theme.accent);
    g.fillEllipse(dotArea.getX(), dotArea.getY() + 4.0f, r * 2.0f, r * 2.0f);
    g.setColour(theme.accentAlt);
    g.fillEllipse(dotArea.getCentreX() - r, dotArea.getY(), r * 2.0f, r * 2.0f);
    g.setColour(theme.mutedText);
    g.fillEllipse(dotArea.getRight() - r * 2.0f, dotArea.getY() + 4.0f, r * 2.0f, r * 2.0f);
}

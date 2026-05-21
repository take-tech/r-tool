#pragma once

#include <optional>

#include <juce_core/juce_core.h>

namespace rtool::settings
{
    std::optional<int> loadDefaultThemeIndex();
    void saveDefaultThemeIndex(int themeIndex);
}

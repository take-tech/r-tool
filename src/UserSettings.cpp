#include "UserSettings.h"

namespace
{
    juce::File getSettingsFile()
    {
        return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
            .getChildFile("Ranze")
            .getChildFile("R-Tool")
            .getChildFile("settings.xml");
    }
}

namespace rtool::settings
{
    std::optional<int> loadDefaultThemeIndex()
    {
        const auto file = getSettingsFile();

        if (! file.existsAsFile())
            return std::nullopt;

        auto xml = juce::XmlDocument::parse(file);

        if (xml == nullptr || ! xml->hasTagName("settings"))
            return std::nullopt;

        return juce::jlimit(0, 4, xml->getIntAttribute("defaultTheme", 0));
    }

    void saveDefaultThemeIndex(int themeIndex)
    {
        auto file = getSettingsFile();
        file.getParentDirectory().createDirectory();

        juce::XmlElement xml("settings");
        xml.setAttribute("defaultTheme", juce::jlimit(0, 4, themeIndex));
        xml.writeTo(file);
    }
}

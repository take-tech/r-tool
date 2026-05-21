cmake_minimum_required(VERSION 3.22)

project(RTool VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(external/juce)
add_subdirectory(external/ranze-plugin-ui)

if(APPLE)
    set(RTOOL_FORMATS VST3 AU)
else()
    set(RTOOL_FORMATS VST3)
endif()

juce_add_plugin(RTool
    COMPANY_NAME "Ranze"
    BUNDLE_ID "com.ranze.rtool"
    PLUGIN_MANUFACTURER_CODE Rnze
    PLUGIN_CODE RToo
    FORMATS ${RTOOL_FORMATS}
    PRODUCT_NAME "R-Tool"
)

target_sources(RTool
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginEditor.cpp
        src/Parameters.cpp
        src/UserSettings.cpp
        src/dsp/ToolProcessor.cpp
        src/gui/MeterComponent.cpp
)

target_include_directories(RTool
    PRIVATE
        src
)

target_compile_definitions(RTool
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(RTool
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        ranze::plugin_ui
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

cmake_minimum_required (VERSION 3.15...3.27 FATAL_ERROR)

list (APPEND CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_LIST_DIR}/share/cmake/Modules"
)

project(Biquads)

set (PROJECT_VERSION_MAJOR 1)
set (PROJECT_VERSION_MINOR 0)
set (PROJECT_VERSION_PATCH 2)

include (GetSourceInfo)
get_source_info ()
set (PROJECT_VERSION_TWEAK "${git_revision}")
# set(PROJECT_VERSION_TWEAK "x")

set (PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" CACHE STRING "Value given to the ``VERSION`` option of the most recent call to the ``project()`` command, if any." FORCE)

set (PROJECT_HOMEPAGE_URL "https://stoneydsp.github.io/Biquads" CACHE STRING "The homepage URL of the project." FORCE)

file (COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/Components/AutoButton.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/Components/AutoComboBox.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/Components/AutoComponent.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/Components/AutoKnob.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/Components/Viewer.h"
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/Components"
)

file (COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/Modules/Biquads.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/Modules/Coefficient.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/Modules/SecondOrderBiquads.h"
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/Modules"
)

file (COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/PluginEditor.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/PluginParameters.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/PluginProcessor.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/PluginWrapper.h"
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}"
)

# If you've installed JUCE somehow (via a package manager, or directly using the CMake install
# target), you'll need to tell this project that it depends on the installed copy of JUCE. If you've
# included JUCE directly in your source tree (perhaps as a submodule), you'll need to tell CMake to
# include that subdirectory as part of the build.

# If you've installed JUCE to your system
find_package (JUCE CONFIG REQUIRED)
# or
# If you've put JUCE in a subdirectory called JUCE
# add_subdirectory(JUCE)


# `juce_add_plugin` adds a static library target with the name passed as the first argument
# (AudioPluginExample here). This target is a normal CMake target, but has a lot of extra properties set
# up by default. As well as this shared code static library, this function adds targets for each of
# the formats specified by the FORMATS arguments. This function accepts many optional arguments.
# Check the readme at `docs/CMake API.md` in the JUCE repo for the full list.

juce_add_plugin (Biquads
    # Set this if the plugin version is different to the project version
    # VERSION ...
    # "ICON_*" arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_BIG ...
    # ICON_SMALL ...
    # Specify the name of the plugin's author
    COMPANY_NAME StoneyDSP
    # Is this a synth or an effect?
    IS_SYNTH FALSE
    # Does the plugin need midi input?
    NEEDS_MIDI_INPUT FALSE
    # Does the plugin need midi output?
    NEEDS_MIDI_OUTPUT FALSE
    # Is this plugin a MIDI effect?
    IS_MIDI_EFFECT FALSE
    # Does the editor need keyboard focus?
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    # Should the plugin be installed to a default location after building?
    COPY_PLUGIN_AFTER_BUILD FALSE
    # A four-character manufacturer id with at least one upper-case character
    PLUGIN_MANUFACTURER_CODE Juce
    # A unique four-character plugin id with exactly one upper-case character
    # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    PLUGIN_CODE Dem0
        # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    FORMATS AU VST3 Standalone
    # The name of the final executable, which can differ from the target name
    PRODUCT_NAME "Biquads"
)

# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into your build tree. This should be included with `#include <JuceHeader.h>`. The include path for
# this header will be automatically added to the target. The main function of the JuceHeader is to
# include all your JUCE module headers; if you're happy to include module headers directly, you
# probably don't need to call this.
juce_generate_juce_header (Biquads)

target_include_directories (Biquads

    PRIVATE

        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_sources (Biquads

    PUBLIC

        FILE_SET biquads_HEADERS
        TYPE HEADERS

        BASE_DIRS
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}>
            $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
        FILES
            # # include/${PROJECT_NAME}/Components/AutoButton.h
            # $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/Components/AutoButton.h>
            # $<INSTALL_INTERFACE:include/${PROJECT_NAME}/Components/AutoButton.h>

            # # include/${PROJECT_NAME}/Components/AutoComboBox.h
            # $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/Components/AutoComboBox.h>
            # $<INSTALL_INTERFACE:include/${PROJECT_NAME}/Components/AutoComboBox.h>

            # include/${PROJECT_NAME}/Components/AutoComponent.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/Components/AutoComponent.h>
            $<INSTALL_INTERFACE:include/${PROJECT_NAME}/Components/AutoComponent.h>

            # # include/${PROJECT_NAME}/Components/AutoKnob.h
            # $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/Components/AutoKnob.h>
            # $<INSTALL_INTERFACE:include/${PROJECT_NAME}/Components/AutoKnob.h>

            # include/${PROJECT_NAME}/Components/Viewer.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/Components/Viewer.h>
            $<INSTALL_INTERFACE:include/${PROJECT_NAME}/Components/Viewer.h>

            # include/${PROJECT_NAME}/Modules/Biquads.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/Modules/Biquads.h>
            $<INSTALL_INTERFACE:include/${PROJECT_NAME}/Modules/Biquads.h>

            # include/${PROJECT_NAME}/Modules/Coefficient.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/Modules/Coefficient.h>
            $<INSTALL_INTERFACE:include/${PROJECT_NAME}/Modules/Coefficient.h>

            # include/${PROJECT_NAME}/Modules/SecondOrderBiquads.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/Modules/SecondOrderBiquads.h>
            $<INSTALL_INTERFACE:include/${PROJECT_NAME}/Modules/SecondOrderBiquads.h>

            # include/${PROJECT_NAME}/PluginEditor.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/PluginEditor.h>
            $<INSTALL_INTERFACE:include/${PROJECT_NAME}/PluginEditor.h>

            # include/${PROJECT_NAME}/PluginParameters.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/PluginParameters.h>
            $<INSTALL_INTERFACE:include/${PROJECT_NAME}/PluginParameters.h>

            # include/${PROJECT_NAME}/PluginProcessor.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/PluginProcessor.h>
            $<INSTALL_INTERFACE:include/${PROJECT_NAME}/PluginProcessor.h>

            # include/${PROJECT_NAME}/PluginWrapper.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/PluginWrapper.h>
            $<INSTALL_INTERFACE:include/${PROJECT_NAME}/PluginWrapper.h>

)

target_sources (Biquads
    PRIVATE
        include/Biquads/PluginEditor.cpp
        include/Biquads/PluginParameters.cpp
        include/Biquads/PluginProcessor.cpp
        include/Biquads/PluginWrapper.cpp

        include/Biquads/Modules/Biquads.cpp
        include/Biquads/Modules/Coefficient.cpp
        include/Biquads/Modules/SecondOrderBiquads.cpp

        # include/Biquads/Components/AutoButton.cpp
        # include/Biquads/Components/AutoComboBox.cpp
        include/Biquads/Components/AutoComponent.cpp
        # include/Biquads/Components/AutoKnob.cpp
        include/Biquads/Components/Viewer.cpp
)

target_compile_definitions (Biquads
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_WEB_BROWSER=0
        # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries (Biquads
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

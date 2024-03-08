#[=============================================================================[
This file is part of the StoneyDSP library.
Copyright (c) 2024 - StoneyDSP
Home: https://www.stoneydsp.com
Source: https://github.com/StoneyDSP/StoneyDSP

StoneyDSP is an open source library subject to open-source licensing.

By using StoneyDSP, you agree to the terms of the StoneyDSP End-User License
Agreement and also the StoneyDSP Privacy Policy.

End User License Agreement: www.stoneydsp.com/LICENSE
Privacy Policy: www.stoneydsp.com/privacy-policy

By using StoneyDSP, you must also agree to the terms of both the JUCE 7 End-User
License Agreement and JUCE Privacy Policy.

End User License Agreement: www.juce.com/juce-7-licence
Privacy Policy: www.juce.com/juce-privacy-policy

Or: You may also use this code under the terms of the GPL v3 (see
www.gnu.org/licenses).

STONEYDSP IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
DISCLAIMED.
]=============================================================================]#

cmake_minimum_required (VERSION 3.22...3.28 FATAL_ERROR)

project (STONEYDSP_BIQUADS VERSION 1.1.0)

#find_package (JUCE CONFIG REQUIRED)
#find_package (StoneyDSP CONFIG REQUIRED)

juce_add_plugin (Biquads
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME StoneyDSP                      # Specify the name of the plugin's author
    IS_SYNTH FALSE                              # Is this a synth or an effect?
    NEEDS_MIDI_INPUT FALSE                      # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT FALSE                     # Does the plugin need midi output?
    IS_MIDI_EFFECT FALSE                        # Is this plugin a MIDI effect?
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE            # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD FALSE               # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE Sdsp               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Biq0                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "Biquads"                      # The name of the final executable, which can differ from the target name
)

juce_generate_juce_header (Biquads)

target_include_directories (Biquads
    PRIVATE
        $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set (STONEYDSP_BIQUADS_GUI_UNITS "")
list (APPEND STONEYDSP_BIQUADS_GUI_UNITS
    # "StoneyDSP/Biquads/Components/AutoButton"
    # "StoneyDSP/Biquads/Components/AutoComboBox"
    # "StoneyDSP/Biquads/Components/AutoKnob"
    "StoneyDSP/Biquads/Components/AutoComponent"
    "StoneyDSP/Biquads/Components/Viewer"
)

set (STONEYDSP_BIQUADS_DSP_UNITS "")
list (APPEND STONEYDSP_BIQUADS_DSP_UNITS
    "StoneyDSP/Biquads/Modules/Biquads"
    "StoneyDSP/Biquads/Modules/Coefficient"
    "StoneyDSP/Biquads/Modules/SecondOrderBiquads"
)

set (STONEYDSP_BIQUADS_APP_UNITS "")
list (APPEND STONEYDSP_BIQUADS_APP_UNITS
    "StoneyDSP/Biquads/PluginEditor"
    "StoneyDSP/Biquads/PluginParameters"
    "StoneyDSP/Biquads/PluginProcessor"
    "StoneyDSP/Biquads/PluginWrapper"
)

set (STONEYDSP_BIQUADS_UNITS "")
list (APPEND STONEYDSP_BIQUADS_UNITS
    ${STONEYDSP_BIQUADS_GUI_UNITS}
    ${STONEYDSP_BIQUADS_DSP_UNITS}
    ${STONEYDSP_BIQUADS_APP_UNITS}
)

foreach (STONEYDSP_BIQUADS_UNIT IN LISTS STONEYDSP_BIQUADS_UNITS)

    string (REGEX REPLACE "[/]" "_" _stoneydsp_biquads_unit_transformed "${STONEYDSP_BIQUADS_UNIT}")
    # set (_stoneydsp_biquads_unit_transformed ${_stoneydsp_biquads_unit_transformed})
    string (TOUPPER "${_stoneydsp_biquads_unit_transformed}" _stoneydsp_biquads_unit_transformed_upper)

    message (STATUS "Biquads: ${_stoneydsp_biquads_unit_transformed_upper} found at ${_stoneydsp_biquads_unit_transformed}")

    configure_file (
        "include/${STONEYDSP_BIQUADS_UNIT}.h" # SRC_DIR
        "include/${STONEYDSP_BIQUADS_UNIT}.h" # BIN_DIR
    )
    target_sources (Biquads
        PUBLIC
            FILE_SET biquads_HEADERS
            TYPE HEADERS
            BASE_DIRS
                $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/include>
                $<INSTALL_INTERFACE:include>
            FILES
                $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/include/${STONEYDSP_BIQUADS_UNIT}.h>
                $<INSTALL_INTERFACE:include/${STONEYDSP_BIQUADS_UNIT}.h>
    )
    target_sources (Biquads
        PRIVATE
            "src/${STONEYDSP_BIQUADS_UNIT}.cpp"
    )
    # target_compile_definitions (Biquads
    #     PRIVATE
    #         "${STONEYDSP_BIQUADS_UNIT_UPPERCASE}_H_INCLUDED"
    # )
endforeach ()

target_compile_definitions (Biquads
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries (Biquads
    PRIVATE
        # StoneyDSP::stoneydsp_core
        # StoneyDSP::stoneydsp_audio
        # StoneyDSP::stoneydsp_graphics

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

option (STONEYDSP_BIQUADS_BUILD_TESTS "Build tests" ON)
if (STONEYDSP_BIQUADS_BUILD_TESTS AND (STONEYDSP_BIQUADS_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))

    enable_testing ()
    add_subdirectory (tests)
endif ()

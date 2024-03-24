#[=============================================================================[
#   Simple two-pole equalizer with variable oversampling.
#   Copyright (c) 2024 - Nathan J. Hood
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
#]=============================================================================]

cmake_minimum_required (VERSION 3.22...3.29 FATAL_ERROR)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/share/cmake/Modules")

if (NOT DEFINED CMAKE_CONFIGURATION_TYPES)
    set (CMAKE_CONFIGURATION_TYPES "" CACHE STRING "Specifies the available build types (configurations) on multi-config generators." FORCE)
    list (APPEND CMAKE_CONFIGURATION_TYPES "Debug" "Release")
endif ()

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "Specifies the build type on single-configuration generators." FORCE)
endif ()

set_property (CACHE CMAKE_CONFIGURATION_TYPES PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})
set_property (CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})

include (GetGitRevListCount)
get_git_rev_list_count ()

set(CPACK_PACKAGE_VENDOR "StoneyDSP")

project (STONEYDSP_BIQUADS
    VERSION 1.2.3.${_git_rev_list_count}
    DESCRIPTION "Simple two-pole equalizer with variable oversampling."
    HOMEPAGE_URL "https://github.com/nathanjhood/Biquads"
    LANGUAGES C;CXX
)

include (StoneyDSPBiquadsWriteVersionFile)
stoneydsp_biquads_write_version_file ()
unset (_git_rev_list_count)

message (STATUS "Configuring StoneyDSP Biquads v${STONEYDSP_BIQUADS_VERSION}...")

set_property(GLOBAL PROPERTY USE_FOLDERS YES)
set_property(GLOBAL PROPERTY DONT_SET_USING_JUCE_NAMESPACE 1)

if (NOT DEFINED JUCE_VERSION)
    message (STATUS "Looking for JUCE...")
    set (JUCE_BUILD_EXAMPLES FALSE)
    set (JUCE_BUILD_EXTRAS   FALSE)
    find_package (JUCE CONFIG REQUIRED)
    message (STATUS "  Found JUCE v${JUCE_VERSION}...")
endif ()

option (STONEYDSP_BIQUADS_BUILD_TESTS "Collect Tracktion Pluginval and configure with CTest (requires <CURL> and <7ZIP>)." ON)
option (STONEYDSP_BIQUADS_TARGET_IS_INSTALLABLE "" OFF)
option (STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_CONFIG_FLAGS "Link with juce::juce_recommended_config_flags." ON)
option (STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_LTO_FLAGS "Link with juce::juce_recommended_lto_flags." ON)
option (STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_WARNING_FLAGS "Link with juce::juce_recommended_warning_flags." ON)

#[=============================================================================[
#   target: StoneyDSP::stoneydsp_core
#   target: StoneyDSP::stoneydsp_audio
#]=============================================================================]

# find_package (StoneyDSP CONFIG REQUIRED)

add_subdirectory (Modules)

#[=============================================================================[
#   target: StoneyDSP::Biquads_Audio_Plugin_Dependencies
#]=============================================================================]

set_directory_properties (PROPERTIES

    # JUCE_COPY_PLUGIN_AFTER_BUILD TRUE
    # JUCE_VST_COPY_DIR
    # JUCE_VST3_COPY_DIR
    # JUCE_AAX_COPY_DIR
    # JUCE_AU_COPY_DIR
    # JUCE_UNITY_COPY_DIR

    JUCE_COMPANY_NAME       "StoneyDSP"
    JUCE_COMPANY_WEBSITE    "https://stoneydsp.com"
    JUCE_COMPANY_EMAIL      "nathanjhood@googlemail.com"
    JUCE_COMPANY_COPYRIGHT  "Copyright (c) 2024 - Nathan J. Hood <nathanjhood@googlemail.com>"
)

set (STONEYDSP_BIQUADS_INCLUDEDIR "")
set (STONEYDSP_BIQUADS_INCLUDEDIR "${STONEYDSP_BIQUADS_BINARY_DIR}/Sources") # dont CACHE FORCE this yet, it will change again soon...

set (STONEYDSP_BIQUADS_FILE_EXTENSIONS "")
list (APPEND STONEYDSP_BIQUADS_FILE_EXTENSIONS "hpp" "cpp" "mm")

set (STONEYDSP_BIQUADS_UNIT             "Biquads")
set (STONEYDSP_BIQUADS_UNIT_EDITOR      "Editor")
set (STONEYDSP_BIQUADS_UNIT_PARAMETERS  "Parameters")
set (STONEYDSP_BIQUADS_UNIT_PROCESSOR   "Processor")
set (STONEYDSP_BIQUADS_UNIT_WRAPPER     "Wrapper")

set (STONEYDSP_BIQUADS_UNIT_FILES "")
list (APPEND STONEYDSP_BIQUADS_UNIT_FILES
    # "${STONEYDSP_BIQUADS_UNIT}"
    "${STONEYDSP_BIQUADS_UNIT_EDITOR}"
    "${STONEYDSP_BIQUADS_UNIT_PARAMETERS}"
    "${STONEYDSP_BIQUADS_UNIT_PROCESSOR}"
    "${STONEYDSP_BIQUADS_UNIT_WRAPPER}"
)
foreach (STONEYDSP_BIQUADS_UNIT_FILE IN LISTS STONEYDSP_BIQUADS_UNIT_FILES)
    foreach (STONEYDSP_BIQUADS_FILE_EXTENSION IN LISTS STONEYDSP_BIQUADS_FILE_EXTENSIONS)
        configure_file (
            "Sources/${STONEYDSP_BIQUADS_UNIT_FILE}/${STONEYDSP_BIQUADS_UNIT_FILE}.${STONEYDSP_BIQUADS_FILE_EXTENSION}" # SRC_DIR
            "Biquads_Audio_Plugin_artefacts/Sources/${STONEYDSP_BIQUADS_UNIT_FILE}/${STONEYDSP_BIQUADS_UNIT_FILE}.${STONEYDSP_BIQUADS_FILE_EXTENSION}" # BIN_DIR
        )
    endforeach ()
endforeach()

set (STONEYDSP_BIQUADS_EXPORT_TARGETS "")

foreach (STONEYDSP_BIQUADS_UNIT_FILE IN LISTS STONEYDSP_BIQUADS_UNIT_FILES)

    add_library (${STONEYDSP_BIQUADS_UNIT_FILE} INTERFACE)
    add_library (StoneyDSP::Biquads::${STONEYDSP_BIQUADS_UNIT_FILE} ALIAS ${STONEYDSP_BIQUADS_UNIT_FILE})

    source_group (${STONEYDSP_BIQUADS_UNIT_FILE}
        FILES
            "${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources/${STONEYDSP_BIQUADS_UNIT_FILE}/${STONEYDSP_BIQUADS_UNIT_FILE}.hpp"
            "${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources/${STONEYDSP_BIQUADS_UNIT_FILE}/${STONEYDSP_BIQUADS_UNIT_FILE}.cpp"
            "${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources/${STONEYDSP_BIQUADS_UNIT_FILE}/${STONEYDSP_BIQUADS_UNIT_FILE}.mm"
    )

    target_include_directories (${STONEYDSP_BIQUADS_UNIT_FILE}
        INTERFACE
            $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources>
            $<INSTALL_INTERFACE:include/StoneyDSP/Biquads>
    )

    if (CMAKE_MAJOR_VERSION GREATER_EQUAL 3 AND (CMAKE_MINOR_VERSION LESS 23))

        target_include_directories (${STONEYDSP_BIQUADS_UNIT_FILE}
            INTERFACE
                $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources/${STONEYDSP_BIQUADS_UNIT_FILE}>
                $<INSTALL_INTERFACE:include/StoneyDSP/Biquads/${STONEYDSP_BIQUADS_UNIT_FILE}>
        )

    else ()

        target_sources (${STONEYDSP_BIQUADS_UNIT_FILE}
            INTERFACE
                FILE_SET biquads_${STONEYDSP_BIQUADS_UNIT_FILE}_HEADERS
                TYPE HEADERS
                BASE_DIRS
                    $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources/${STONEYDSP_BIQUADS_UNIT_FILE}>
                    $<INSTALL_INTERFACE:include/StoneyDSP/Biquads/${STONEYDSP_BIQUADS_UNIT_FILE}>
                FILES
                    $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources/${STONEYDSP_BIQUADS_UNIT_FILE}/${STONEYDSP_BIQUADS_UNIT_FILE}.hpp>
                    $<INSTALL_INTERFACE:include/StoneyDSP/Biquads/${STONEYDSP_BIQUADS_UNIT_FILE}/${STONEYDSP_BIQUADS_UNIT_FILE}.hpp>
        )

    endif()

    # if (STONEYDSP_BIQUADS_MODULE)
    target_sources (${STONEYDSP_BIQUADS_UNIT_FILE}
        INTERFACE
            $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources/${STONEYDSP_BIQUADS_UNIT_FILE}/${STONEYDSP_BIQUADS_UNIT_FILE}.mm>
            $<INSTALL_INTERFACE:include/StoneyDSP/Biquads/${STONEYDSP_BIQUADS_UNIT_FILE}/${STONEYDSP_BIQUADS_UNIT_FILE}.mm>
    )
    # endif ()

    target_sources (${STONEYDSP_BIQUADS_UNIT_FILE}
        INTERFACE
            $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources/${STONEYDSP_BIQUADS_UNIT_FILE}/${STONEYDSP_BIQUADS_UNIT_FILE}.cpp>
            $<INSTALL_INTERFACE:include/StoneyDSP/Biquads/${STONEYDSP_BIQUADS_UNIT_FILE}/${STONEYDSP_BIQUADS_UNIT_FILE}.cpp>
    )

    list (APPEND STONEYDSP_BIQUADS_EXPORT_TARGETS ${STONEYDSP_BIQUADS_UNIT_FILE})

endforeach ()

target_link_libraries (Wrapper INTERFACE StoneyDSP::Biquads::Parameters)

target_link_libraries (Processor INTERFACE StoneyDSP::Biquads::Wrapper StoneyDSP::Biquads::Parameters StoneyDSP::Biquads::Editor)

unset (STONEYDSP_BIQUADS_INCLUDEDIR)
set (STONEYDSP_BIQUADS_INCLUDEDIR
    $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources>
    $<INSTALL_INTERFACE:include/StoneyDSP/Biquads>
)

juce_add_plugin (Biquads_Audio_Plugin
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...

    VST3_AUTO_MANIFEST FALSE
    BUNDLE_ID "com.StoneyDSP.Biquads"

    MICROPHONE_PERMISSION_ENABLED FALSE
    CAMERA_PERMISSION_ENABLED FALSE
    BLUETOOTH_PERMISSION_ENABLED FALSE

    VST2_CATEGORY "kPlugCategEffect"
    VST3_CATEGORIES "Fx" "EQ"

    IS_SYNTH FALSE                              # Is this a synth or an effect?
    NEEDS_MIDI_INPUT FALSE                      # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT FALSE                     # Does the plugin need midi output?
    IS_MIDI_EFFECT FALSE                        # Is this plugin a MIDI effect?
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE           # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD FALSE               # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE Manu               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Jer3                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3

    PRODUCT_NAME "Biquads"                      # The name of the final executable, which can differ from the target name
    COMPANY_NAME "StoneyDSP"                    # Specify the name of the plugin's author
    COMPANY_WEBSITE "https://stoneydsp.com"
    COMPANY_EMAIL "nathanjhood@googlemail.com"
    COMPANY_COPYRIGHT "Copyright (c) 2024 - Nathan J. Hood <nathanjhood@googlemail.com>"

    # COPY_PLUGIN_AFTER_BUILD
    # VST_COPY_DIR
    # VST3_COPY_DIR
    # AAX_COPY_DIR
    # AU_COPY_DIR
    # UNITY_COPY_DIR
)

target_compile_features(Biquads_Audio_Plugin PUBLIC cxx_std_17)

set (STONEYDSP_BIQUADS_JUCE_MODULE_DEPENDENCIES "")
list(APPEND STONEYDSP_BIQUADS_JUCE_MODULE_DEPENDENCIES
    juce_audio_basics
    juce_audio_devices
    juce_audio_formats
    juce_audio_plugin_client
    juce_audio_processors
    juce_audio_utils
    juce_core
    juce_data_structures
    juce_dsp
    juce_events
    juce_graphics
    juce_gui_basics
    juce_gui_extra
)
foreach(STONEYDSP_BIQUADS_JUCE_MODULE_DEPENDENCY IN LISTS STONEYDSP_BIQUADS_JUCE_MODULE_DEPENDENCIES)
    target_link_libraries (Biquads_Audio_Plugin
        PRIVATE
            juce::${STONEYDSP_BIQUADS_JUCE_MODULE_DEPENDENCY}
    )
endforeach()

set (STONEYDSP_BIQUADS_STONEYDSP_MODULE_DEPENDENCIES "")
list (APPEND STONEYDSP_BIQUADS_STONEYDSP_MODULE_DEPENDENCIES
    stoneydsp_core
    stoneydsp_audio
    # stoneydsp_graphics
)
foreach(STONEYDSP_BIQUADS_STONEYDSP_MODULE_DEPENDENCY IN LISTS STONEYDSP_BIQUADS_STONEYDSP_MODULE_DEPENDENCIES)
    target_link_libraries (Biquads_Audio_Plugin
        PRIVATE
            StoneyDSP::${STONEYDSP_BIQUADS_STONEYDSP_MODULE_DEPENDENCY}
    )
endforeach()

configure_file (
    "Sources/Biquads.hpp"
    "Biquads_Audio_Plugin_artefacts/Sources/Biquads.hpp"
)
configure_file (
    "Sources/Biquads.cpp"
    "Biquads_Audio_Plugin_artefacts/Sources/Biquads.cpp"
)
configure_file (
    "Sources/Biquads.mm"
    "Biquads_Audio_Plugin_artefacts/Sources/Biquads.mm"
)

source_group ("Biquads"
    FILES
        "${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources/Biquads.hpp"
        "${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources/Biquads.cpp"
        "${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources/Biquads.mm"
)

if (CMAKE_MAJOR_VERSION GREATER_EQUAL 3 AND (CMAKE_MINOR_VERSION LESS 23))

    target_include_directories (Biquads_Audio_Plugin
        PUBLIC
            $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources>
            $<INSTALL_INTERFACE:include/StoneyDSP/Biquads>
    )

else ()

    target_sources (Biquads_Audio_Plugin

        PUBLIC
            FILE_SET biquads_AUDIO_PLUGIN_HEADERS
            TYPE HEADERS
            BASE_DIRS
                $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources>
                $<INSTALL_INTERFACE:include/StoneyDSP>
            FILES
                $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources/${STONEYDSP_BIQUADS_UNIT}.hpp>
                $<INSTALL_INTERFACE:include/StoneyDSP/${STONEYDSP_BIQUADS_UNIT}.hpp>
    )
endif()

target_sources (Biquads_Audio_Plugin
    PRIVATE
        $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources/${STONEYDSP_BIQUADS_UNIT}.cpp>
        $<INSTALL_INTERFACE:include/StoneyDSP/${STONEYDSP_BIQUADS_UNIT}.cpp>
)



# if (CMAKE_MAJOR_VERSION GREATER_EQUAL 3 AND (CMAKE_MINOR_VERSION LESS 23))

#     target_include_directories (Biquads_Audio_Plugin
#         PUBLIC
#             $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Sources>
#             $<INSTALL_INTERFACE:include/StoneyDSP/Biquads>
#     )

# else ()

#     target_sources (Biquads_Audio_Plugin

#         PUBLIC
#             FILE_SET biquads_AUDIO_PLUGIN_HEADERS
#             TYPE HEADERS
#             BASE_DIRS
#                 $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Sources>
#                 $<INSTALL_INTERFACE:include/StoneyDSP/Biquads>
#             FILES
#                 $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Sources/${STONEYDSP_BIQUADS_UNIT_PARAMETERS}.hpp>
#                 $<INSTALL_INTERFACE:include/StoneyDSP/Biquads/${STONEYDSP_BIQUADS_UNIT_PARAMETERS}.hpp>

#                 $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Sources/${STONEYDSP_BIQUADS_UNIT_WRAPPER}.hpp>
#                 $<INSTALL_INTERFACE:include/StoneyDSP/Biquads/${STONEYDSP_BIQUADS_UNIT_WRAPPER}.hpp>

#                 $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Sources/${STONEYDSP_BIQUADS_UNIT_PROCESSOR}.hpp>
#                 $<INSTALL_INTERFACE:include/StoneyDSP/Biquads/${STONEYDSP_BIQUADS_UNIT_PROCESSOR}.hpp>

#                 $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Sources/${STONEYDSP_BIQUADS_UNIT_EDITOR}.hpp>
#                 $<INSTALL_INTERFACE:include/StoneyDSP/Biquads/${STONEYDSP_BIQUADS_UNIT_EDITOR}.hpp>

#                 $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Sources/${STONEYDSP_BIQUADS_UNIT}.hpp>
#                 $<INSTALL_INTERFACE:include/StoneyDSP/Biquads/${STONEYDSP_BIQUADS_UNIT}.hpp>
#     )
# endif()

# target_sources (Biquads_Audio_Plugin
#     PRIVATE
#         "Sources/${STONEYDSP_BIQUADS_UNIT_PARAMETERS}.cpp"
#         "Sources/${STONEYDSP_BIQUADS_UNIT_WRAPPER}.cpp"
#         "Sources/${STONEYDSP_BIQUADS_UNIT_PROCESSOR}.cpp"
#         "Sources/${STONEYDSP_BIQUADS_UNIT_EDITOR}.cpp"
#         "Sources/${STONEYDSP_BIQUADS_UNIT}.cpp"
# )

set (STONEYDSP_BIQUADS_JUCE_FLAGS "")
if (STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_CONFIG_FLAGS)
    list (APPEND STONEYDSP_BIQUADS_JUCE_FLAGS juce_recommended_config_flags)
endif ()
if (STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_LTO_FLAGS)
    list (APPEND STONEYDSP_BIQUADS_JUCE_FLAGS juce_recommended_lto_flags)
endif ()
if (STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_WARNING_FLAGS)
    list (APPEND STONEYDSP_BIQUADS_JUCE_FLAGS juce_recommended_warning_flags)
endif ()

foreach (STONEYDSP_BIQUADS_JUCE_FLAG IN LISTS STONEYDSP_BIQUADS_JUCE_FLAGS)
    target_link_libraries (Biquads_Audio_Plugin
        PRIVATE
            juce::${STONEYDSP_BIQUADS_JUCE_FLAG}
    )
endforeach ()

target_compile_definitions (Biquads_Audio_Plugin
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        DONT_SET_USING_JUCE_NAMESPACE=1
)

#[=============================================================================[
#   target: StoneyDSP::Biquads_Audio_Plugin
#]=============================================================================]

# juce_add_modules(
#     ALIAS_NAMESPACE StoneyDSP::Biquads
#     INSTALL_PATH "include/StoneyDSP/Biquads"
#     "${STONEYDSP_BIQUADS_SOURCE_DIR}/include/Biquads/Parameters"
#     "${STONEYDSP_BIQUADS_SOURCE_DIR}/include/Biquads/Wrapper"
#     "${STONEYDSP_BIQUADS_SOURCE_DIR}/include/Biquads/Editor"
#     "${STONEYDSP_BIQUADS_SOURCE_DIR}/include/Biquads/Processor"
# )


# target_link_libraries (Biquads_Audio_Plugin
#     PRIVATE
        # StoneyDSP::Biquads_Audio_Plugin_Dependencies # MUST be PRIVATE!
        # StoneyDSP::Biquads::Parameters
        # StoneyDSP::Biquads::Wrapper
        # StoneyDSP::Biquads::Editor
        # StoneyDSP::Biquads::Processor
    #     StoneyDSP::Biquads_Audio_Plugin_Library
    # INTERFACE
    #     StoneyDSP::Biquads_Audio_Plugin_Interface # MUST be INTERFACE!
# )

#[=============================================================================[
#   Helpers
#]=============================================================================]

juce_generate_juce_header       (Biquads_Audio_Plugin)

juce_enable_vst3_manifest_step  (Biquads_Audio_Plugin)

#[=============================================================================[
#   CTest configuration
#]=============================================================================]

if (STONEYDSP_BIQUADS_BUILD_TESTS AND (STONEYDSP_BIQUADS_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    enable_testing ()
    add_subdirectory (Testing)
endif ()

#[=============================================================================[
#   Export configuration
#]=============================================================================]

export (
    TARGETS ${STONEYDSP_BIQUADS_EXPORT_TARGETS}
    NAMESPACE StoneyDSP::Biquads::
    FILE StoneyDSPBiquadsTargets.cmake
)

foreach (STONEYDSP_BIQUADS_EXPORT_TARGET IN LISTS STONEYDSP_BIQUADS_EXPORT_TARGETS)
    if (CMAKE_MAJOR_VERSION GREATER_EQUAL 3 AND (CMAKE_MINOR_VERSION LESS 23))
        install (TARGETS ${STONEYDSP_BIQUADS_EXPORT_TARGET}
            EXPORT StoneyDSPBiquadsTargets
            LIBRARY DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
            ARCHIVE DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
            RUNTIME DESTINATION  "${CMAKE_INSTALL_BINDIR}"
            INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP/Biquads"
            PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP/Biquads/${STONEYDSP_BIQUADS_EXPORT_TARGET}"
        )
    else ()
        install (TARGETS ${STONEYDSP_BIQUADS_EXPORT_TARGET}
            EXPORT StoneyDSPBiquadsTargets
            LIBRARY DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
            ARCHIVE DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
            RUNTIME DESTINATION  "${CMAKE_INSTALL_BINDIR}"
            INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP/Biquads"
            FILE_SET biquads_${STONEYDSP_BIQUADS_EXPORT_TARGET}_HEADERS
        )
    endif ()
endforeach ()

install(
  EXPORT StoneyDSPBiquadsTargets
  FILE StoneyDSPBiquadsTargets.cmake
  NAMESPACE StoneyDSP::Biquads::
  DESTINATION "lib/cmake/StoneyDSP/Biquads"
)

#[=============================================================================[
#   Install & CPack configuration
#]=============================================================================]

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

write_basic_package_version_file(
    "${STONEYDSP_BIQUADS_BINARY_DIR}/StoneyDSPBiquadsConfigVersion.cmake"
    VERSION ${STONEYDSP_BIQUADS_VERSION}
    COMPATIBILITY ExactVersion
)

unset(STONEYDSP_BIQUADS_INSTALL_LIBDIR)
unset(STONEYDSP_BIQUADS_INCLUDEDIR)

set(STONEYDSP_BIQUADS_INSTALL_BINDIR   ${CMAKE_INSTALL_BINDIR} CACHE STRING "The location, relative to the install prefix, where the StoneyDSP Biquads config file will be installed")
set(STONEYDSP_BIQUADS_INSTALL_LIBDIR   ${CMAKE_INSTALL_LIBDIR} CACHE STRING "The location, relative to the install prefix, where the StoneyDSP Biquads config file will be installed")
set(STONEYDSP_BIQUADS_INCLUDEDIR       ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "Installation directory for include files, a relative path that will be joined with ${CMAKE_INSTALL_PREFIX} or an absolute path." FORCE)

set(STONEYDSP_BIQUADS_INSTALL_CMAKEDIR "${STONEYDSP_BIQUADS_INSTALL_LIBDIR}/cmake/StoneyDSP/Biquads" CACHE STRING "The location, relative to the install prefix, where the StoneyDSP Biquads config file will be installed")

# foreach (STONEYDSP_BIQUADS_UNIT_FILE IN LISTS STONEYDSP_BIQUADS_UNIT_FILES)
#     install(FILES
#         "${STONEYDSP_BIQUADS_BINARY_DIR}/include/StoneyDSP/Biquads/${STONEYDSP_BIQUADS_UNIT_FILE}.hpp"
#         "${STONEYDSP_BIQUADS_BINARY_DIR}/include/StoneyDSP/Biquads/${STONEYDSP_BIQUADS_UNIT_FILE}.cpp"
#         "${STONEYDSP_BIQUADS_BINARY_DIR}/include/StoneyDSP/Biquads/${STONEYDSP_BIQUADS_UNIT_FILE}.mm"
#         DESTINATION "include/StoneyDSP/Biquads"
#     )
# endforeach ()

configure_package_config_file(
    "${STONEYDSP_BIQUADS_SOURCE_DIR}/share/StoneyDSP/Biquads/StoneyDSPBiquadsConfig.cmake.in"
    "${STONEYDSP_BIQUADS_BINARY_DIR}/StoneyDSPBiquadsConfig.cmake"
    INSTALL_DESTINATION "lib/cmake/StoneyDSP/Biquads"
)


# install (
#     DIRECTORY "${STONEYDSP_BIQUADS_BINARY_DIR}/share"
#     DESTINATION "Builds"
# )

install (
    DIRECTORY "${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts"
    DESTINATION "Builds"
)

install (FILES
    "${STONEYDSP_BIQUADS_BINARY_DIR}/StoneyDSPBiquadsConfigVersion.cmake"
    "${STONEYDSP_BIQUADS_BINARY_DIR}/StoneyDSPBiquadsConfig.cmake"
    "${STONEYDSP_BIQUADS_SOURCE_DIR}/share/cmake/Modules/GetGitRevListCount.cmake"
    "${STONEYDSP_BIQUADS_SOURCE_DIR}/share/cmake/Modules/StoneyDSPBiquadsCPackIgnoreList.cmake"
    "${STONEYDSP_BIQUADS_SOURCE_DIR}/share/cmake/Modules/StoneyDSPBiquadsWriteVersionFile.cmake"
    "${STONEYDSP_BIQUADS_SOURCE_DIR}/share/cmake/Modules/JoinPaths.cmake"
    "${STONEYDSP_BIQUADS_SOURCE_DIR}/share/cmake/Modules/WriteVersionHeader.cmake"
    "${STONEYDSP_BIQUADS_SOURCE_DIR}/share/StoneyDSP/Biquads/copyright"
    "${STONEYDSP_BIQUADS_SOURCE_DIR}/VERSION"
    "${STONEYDSP_BIQUADS_SOURCE_DIR}/LICENSE"
    "${STONEYDSP_BIQUADS_SOURCE_DIR}/README.md"
    "${STONEYDSP_BIQUADS_SOURCE_DIR}/Biquads.jucer"
    DESTINATION "lib/cmake/StoneyDSP/Biquads"
)

set(STONEYDSP_BIQUADS_CPACK_GENERATORS "")
list(APPEND STONEYDSP_BIQUADS_CPACK_GENERATORS
    "ZIP"
)
if(UNIX)
    list(APPEND STONEYDSP_BIQUADS_CPACK_GENERATORS
        "TAR"
    )
endif()
if(WIN32)
    list(APPEND STONEYDSP_BIQUADS_CPACK_GENERATORS
        "NSIS"
    )
endif()
include(StoneyDSPBiquadsCPackIgnoreList)
# set(CPACK_PACKAGE_CHECKSUM "${PROJECT_VERSION_TWEAK}") # git rev-parse is a good idea for this...

set(CPACK_PACKAGE_NAME              "StoneyDSP-Biquads-v${STONEYDSP_BIQUADS_VERSION_MAJOR}.${STONEYDSP_BIQUADS_VERSION_MINOR}.${STONEYDSP_BIQUADS_VERSION_PATCH}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-JUCE-v${JUCE_VERSION}")
set(CPACK_PACKAGE_FILE_NAME         "StoneyDSP-Biquads-v${STONEYDSP_BIQUADS_VERSION_MAJOR}.${STONEYDSP_BIQUADS_VERSION_MINOR}.${STONEYDSP_BIQUADS_VERSION_PATCH}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-JUCE-v${JUCE_VERSION}") # Multi-platform CMake API distribution
set(CPACK_SOURCE_PACKAGE_FILE_NAME  "StoneyDSP-Biquads-v${STONEYDSP_BIQUADS_VERSION_MAJOR}.${STONEYDSP_BIQUADS_VERSION_MINOR}.${STONEYDSP_BIQUADS_VERSION_PATCH}-Source") # No system spec as this is un-compiled source file distribution (i.e., the Javascript API)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${STONEYDSP_BIQUADS_DESCRIPTION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${STONEYDSP_BIQUADS_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${STONEYDSP_BIQUADS_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${STONEYDSP_BIQUADS_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_TWEAK "${STONEYDSP_BIQUADS_VERSION_TWEAK}")
set(CPACK_RESOURCE_FILE_LICENSE "${STONEYDSP_BIQUADS_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README  "${STONEYDSP_BIQUADS_SOURCE_DIR}/README.md")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON)
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY ON)
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES ${STONEYDSP_BIQUADS_CPACK_IGNORE_LIST})
include(CPack)

#[=============================================================================[
#
#]=============================================================================]

message (STATUS "Configuring StoneyDSP Biquads v${STONEYDSP_BIQUADS_VERSION} - done.")

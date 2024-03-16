#[=============================================================================[
    Simple two-pole equalizer with variable oversampling.
    Copyright (c) 2024 - Nathan J. Hood

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

]=============================================================================]#

cmake_minimum_required (VERSION 3.22...3.29 FATAL_ERROR)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/share/cmake/Modules")

include (GetGitRevListCount)
get_git_rev_list_count ()

project (STONEYDSP_BIQUADS
    VERSION 1.2.2.${_git_rev_list_count}
    DESCRIPTION "Simple two-pole equalizer with variable oversampling."
    HOMEPAGE_URL "https://github.com/nathanjhood/Biquads"
    LANGUAGES C;CXX
)

include (StoneyDSPBiquadsWriteVersionFile)
stoneydsp_biquads_write_version_file ()
unset (_git_rev_list_count)

message (STATUS "Configuring StoneyDSP Biquads v${STONEYDSP_BIQUADS_VERSION}...")

set_property(GLOBAL PROPERTY USE_FOLDERS YES)

if (NOT DEFINED JUCE_VERSION)
    message (STATUS "Looking for JUCE...")
    set (JUCE_BUILD_EXAMPLES FALSE)
    set (JUCE_BUILD_EXTRAS   FALSE)
    find_package (JUCE CONFIG REQUIRED)
    message (STATUS "  Found JUCE v${JUCE_VERSION}...")
endif ()

set_directory_properties(PROPERTIES
    JUCE_COMPANY_NAME       "StoneyDSP"
    JUCE_COMPANY_WEBSITE    "https://stoneydsp.com"
    JUCE_COMPANY_EMAIL      "nathanjhood@googlemail.com"
    JUCE_COMPANY_COPYRIGHT  "Copyright (c) 2024 - StoneyDSP <nathanjhood@googlemail.com>"
)

option (STONEYDSP_BIQUADS_TARGET_IS_INSTALLABLE "" OFF)
option (STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_CONFIG_FLAGS "" ON)
option (STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_LTO_FLAGS "" ON)
option (STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_WARNING_FLAGS "" ON)

set (STONEYDSP_BIQUADS_TARGETS "")

#find_package (StoneyDSP CONFIG REQUIRED)

#[=============================================================================[
    target: StoneyDSP::stoneydsp_core
    target: StoneyDSP::stoneydsp_audio
]=============================================================================]#

add_subdirectory(modules)

#[=============================================================================[
    target: StoneyDSP::Biquads
]=============================================================================]#

juce_add_plugin (Biquads
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...

    VST3_AUTO_MANIFEST FALSE

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
    COMPANY_NAME "StoneyDSP"                    # Specify the name of the plugin's author
)

set (STONEYDSP_BIQUADS_INCLUDEDIR "")
set (STONEYDSP_BIQUADS_INCLUDEDIR "${STONEYDSP_BIQUADS_BINARY_DIR}/include") # dont CACHE FORCE this yet, it will change again soon...

set (STONEYDSP_BIQUADS_UNIT             "StoneyDSP/Biquads")
set (STONEYDSP_BIQUADS_UNIT_EDITOR      "StoneyDSP/Biquads/Editor")
set (STONEYDSP_BIQUADS_UNIT_PARAMETERS  "StoneyDSP/Biquads/Parameters")
set (STONEYDSP_BIQUADS_UNIT_PROCESSOR   "StoneyDSP/Biquads/Processor")
set (STONEYDSP_BIQUADS_UNIT_WRAPPER     "StoneyDSP/Biquads/Wrapper")

set (STONEYDSP_BIQUADS_UNIT_FILES "")
list (APPEND STONEYDSP_BIQUADS_UNIT_FILES
    "${STONEYDSP_BIQUADS_UNIT}"
    "${STONEYDSP_BIQUADS_UNIT_EDITOR}"
    "${STONEYDSP_BIQUADS_UNIT_PARAMETERS}"
    "${STONEYDSP_BIQUADS_UNIT_PROCESSOR}"
    "${STONEYDSP_BIQUADS_UNIT_WRAPPER}"
)
foreach (STONEYDSP_BIQUADS_UNIT_FILE IN LISTS STONEYDSP_BIQUADS_UNIT_FILES)
    configure_file (
        "include/${STONEYDSP_BIQUADS_UNIT_FILE}.hpp" # SRC_DIR
        "include/${STONEYDSP_BIQUADS_UNIT_FILE}.hpp" # BIN_DIR
    )
endforeach()

unset (STONEYDSP_BIQUADS_INCLUDEDIR)
set (STONEYDSP_BIQUADS_INCLUDEDIR
    $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if (CMAKE_MAJOR_VERSION GREATER_EQUAL 3 AND (CMAKE_MINOR_VERSION LESS 23))
    target_include_directories (Biquads PUBLIC ${STONEYDSP_BIQUADS_INCLUDEDIR})
else ()
    target_sources (Biquads
        PUBLIC
            FILE_SET biquads_HEADERS
            TYPE HEADERS
            BASE_DIRS ${STONEYDSP_BIQUADS_INCLUDEDIR}
            FILES
                $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/include/${STONEYDSP_BIQUADS_UNIT}.hpp>
                $<INSTALL_INTERFACE:include/${STONEYDSP_BIQUADS_UNIT}.hpp>

                $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/include/${STONEYDSP_BIQUADS_UNIT_PARAMETERS}.hpp>
                $<INSTALL_INTERFACE:include/${STONEYDSP_BIQUADS_UNIT_PARAMETERS}.hpp>

                $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/include/${STONEYDSP_BIQUADS_UNIT_WRAPPER}.hpp>
                $<INSTALL_INTERFACE:include/${STONEYDSP_BIQUADS_UNIT_WRAPPER}.hpp>

                $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/include/${STONEYDSP_BIQUADS_UNIT_PROCESSOR}.hpp>
                $<INSTALL_INTERFACE:include/${STONEYDSP_BIQUADS_UNIT_PROCESSOR}.hpp>

                $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/include/${STONEYDSP_BIQUADS_UNIT_EDITOR}.hpp>
                $<INSTALL_INTERFACE:include/${STONEYDSP_BIQUADS_UNIT_EDITOR}.hpp>
    )
endif()

foreach (STONEYDSP_BIQUADS_UNIT_FILE IN LISTS STONEYDSP_BIQUADS_UNIT_FILES)

    string (REGEX REPLACE "[/]" "_" _stoneydsp_biquads_unit_transformed "${STONEYDSP_BIQUADS_UNIT_FILE}")
    string (TOUPPER "${_stoneydsp_biquads_unit_transformed}" _stoneydsp_biquads_unit_transformed_upper)

    message (DEBUG "Biquads: ${_stoneydsp_biquads_unit_transformed_upper} found at ${_stoneydsp_biquads_unit_transformed}")

    if (NOT DEFINED STONEYDSP_BIQUADS_MODULE OR (NOT STONEYDSP_BIQUADS_MODULE))
        target_sources (Biquads
            PRIVATE
                "src/${STONEYDSP_BIQUADS_UNIT_FILE}.cpp"
        )
    endif ()
    # target_compile_definitions (Biquads
    #     PRIVATE
    #         "${_stoneydsp_biquads_unit_transformed_upper}_HPP_INCLUDED"
    # )
    unset (_stoneydsp_biquads_unit_transformed)
    unset (_stoneydsp_biquads_unit_transformed_upper)
endforeach ()

if (DEFINED STONEYDSP_BIQUADS_MODULE AND (STONEYDSP_BIQUADS_MODULE))
    target_sources (Biquads
        PRIVATE
            "src/${STONEYDSP_BIQUADS_UNIT}.cpp"
    )
endif ()

target_compile_definitions (Biquads
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        DONT_SET_USING_JUCE_NAMESPACE=1
)

target_link_libraries (Biquads
    PRIVATE
        StoneyDSP::stoneydsp_core
        StoneyDSP::stoneydsp_audio
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
)
if (STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_CONFIG_FLAGS)
    target_link_libraries (Biquads
        PUBLIC
            juce::juce_recommended_config_flags
    )
endif ()
if (STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_LTO_FLAGS)
    target_link_libraries (Biquads
        PUBLIC
            juce::juce_recommended_lto_flags
    )
endif ()
if (STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_WARNING_FLAGS)
    target_link_libraries (Biquads
        PUBLIC
            juce::juce_recommended_warning_flags
    )
endif ()

list (APPEND STONEYDSP_BIQUADS_TARGETS Biquads)

#[=============================================================================[
    Helpers
]=============================================================================]#

juce_generate_juce_header (Biquads)

juce_enable_vst3_manifest_step (Biquads)

#[=============================================================================[
    CTest configuration
]=============================================================================]#
option (STONEYDSP_BIQUADS_BUILD_TESTS "Build tests" ON)

if (STONEYDSP_BIQUADS_BUILD_TESTS AND (STONEYDSP_BIQUADS_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    enable_testing ()
    add_subdirectory (tests)
endif ()

#[=============================================================================[
    Export targets configuration
]=============================================================================]#

include(CMakePackageConfigHelpers)

if (STONEYDSP_BIQUADS_TARGET_IS_INSTALLABLE)
    export (
        TARGETS ${STONEYDSP_BIQUADS_TARGETS}
        FILE share/cmake/StoneyDSPBiquadsTargets.cmake
        NAMESPACE StoneyDSP::
    )
endif ()
#[=============================================================================[
    Install & CPack configuration
]=============================================================================]#


include(GNUInstallDirs)

write_basic_package_version_file(
    "${STONEYDSP_BIQUADS_BINARY_DIR}/StoneyDSPBiquadsConfigVersion.cmake"
    VERSION ${STONEYDSP_BIQUADS_VERSION}
    COMPATIBILITY ExactVersion
)

set(STONEYDSP_BIQUADS_INSTALL_DESTINATION "lib/cmake/StoneyDSP-Biquads-v${STONEYDSP_BIQUADS_VERSION}" CACHE STRING "The location, relative to the install prefix, where the StoneyDSP Biquads config file will be installed")

unset(STONEYDSP_BIQUADS_INCLUDEDIR)
set(STONEYDSP_BIQUADS_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}" CACHE PATH "Installation directory for include files, a relative path that will be joined with ${CMAKE_INSTALL_PREFIX} or an absolute path." FORCE)

foreach (STONEYDSP_BIQUADS_UNIT_FILE IN LISTS STONEYDSP_BIQUADS_UNIT_FILES)
    if (STONEYDSP_BIQUADS_UNIT_FILE STREQUAL "StoneyDSP/Biquads")
        install(FILES
            "${STONEYDSP_BIQUADS_BINARY_DIR}/include/${STONEYDSP_BIQUADS_UNIT_FILE}.hpp"
            DESTINATION "${STONEYDSP_BIQUADS_INCLUDEDIR}/StoneyDSP"
        )
    else ()
    install(FILES
        "${STONEYDSP_BIQUADS_BINARY_DIR}/include/${STONEYDSP_BIQUADS_UNIT_FILE}.hpp"
        DESTINATION "${STONEYDSP_BIQUADS_INCLUDEDIR}/StoneyDSP/Biquads"
    )
    endif ()
endforeach ()

configure_package_config_file(
    "${STONEYDSP_BIQUADS_SOURCE_DIR}/share/StoneyDSP/Biquads/StoneyDSPBiquadsConfig.cmake.in"
    "${STONEYDSP_BIQUADS_BINARY_DIR}/StoneyDSPBiquadsConfig.cmake"
    INSTALL_DESTINATION "${STONEYDSP_BIQUADS_INSTALL_DESTINATION}"
)

if (STONEYDSP_BIQUADS_TARGET_IS_INSTALLABLE)
    install(
        TARGETS ${STONEYDSP_BIQUADS_TARGETS}
        EXPORT StoneyDSPBiquadsTargets
        LIBRARY DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION  "${CMAKE_INSTALL_BINDIR}"
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP"
        FILE_SET biquads_HEADERS
    )
    install(
        EXPORT StoneyDSPBiquadsTargets
        FILE StoneyDSPBiquadsTargets.cmake
        NAMESPACE StoneyDSP::
        DESTINATION lib/cmake/StoneyDSP
    )
endif ()

install(FILES
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
    DESTINATION "${STONEYDSP_BIQUADS_INSTALL_DESTINATION}"
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
set(CPACK_PACKAGE_VENDOR "StoneyDSP")
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
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP") # Check out CPack's 'NSIS' installer for Win32, and the others!
set(CPACK_SOURCE_IGNORE_FILES ${STONEYDSP_BIQUADS_CPACK_IGNORE_LIST})
include(CPack)

#[=============================================================================[

]=============================================================================]#

message (STATUS "Configuring StoneyDSP Biquads v${STONEYDSP_BIQUADS_VERSION} - done.")

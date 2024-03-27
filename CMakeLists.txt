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

#[=============================================================================[
#   Initialize
#]=============================================================================]

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

#[=============================================================================[
#   Project: STONEYDSP_BIQUADS
#]=============================================================================]

project (STONEYDSP_BIQUADS
    VERSION 1.2.3.${_git_rev_list_count}
    DESCRIPTION "Simple two-pole equalizer with variable oversampling."
    HOMEPAGE_URL "https://github.com/nathanjhood/Biquads"
    LANGUAGES C;CXX
)

set(CPACK_PACKAGE_VENDOR "StoneyDSP")

include (StoneyDSPBiquadsWriteVersionFile)
stoneydsp_biquads_write_version_file ()
unset (_git_rev_list_count)

message (STATUS "Configuring StoneyDSP Biquads v${STONEYDSP_BIQUADS_VERSION}...")

set_property(GLOBAL PROPERTY USE_FOLDERS YES)
set_property(GLOBAL PROPERTY DONT_SET_USING_JUCE_NAMESPACE 1)
# set_property(GLOBAL PROPERTY JUCE_WEB_BROWSER FALSE)

#[=============================================================================[
#   Dependency: JUCE >= v7.0.9
#]=============================================================================]

if (NOT DEFINED JUCE_VERSION)
    message (STATUS "Looking for JUCE...")
    set (JUCE_BUILD_EXAMPLES FALSE)
    set (JUCE_BUILD_EXTRAS   FALSE)
    find_package (JUCE CONFIG REQUIRED)
    message (STATUS "  Found JUCE v${JUCE_VERSION}...")
endif ()

#[=============================================================================[
#   Options
#]=============================================================================]

include (CMakeDependentOption)

cmake_dependent_option  (STONEYDSP_BIQUADS_BUILD_TESTS                          "Collect Tracktion Pluginval and configure with CTest (requires <CURL> and <7ZIP>)." ON STONEYDSP_BIQUADS_IS_TOP_LEVEL OFF)
option                  (STONEYDSP_BIQUADS_GENERATE_JUCE_HEADER                 "Generate a 'JuceHeader.h' file for inclusion. Available by default on all platforms." ON)
option                  (STONEYDSP_BIQUADS_COPY_PLUGIN_AFTER_BUILD              "Copy the built plugin formats specified above to their relevant directories, once the build is complete. Available by default on all platforms. (May require elevated privileges to run.)" OFF)
option                  (STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_CONFIG_FLAGS  "Link with juce::juce_recommended_config_flags. Available by default on all platforms."    ON)
option                  (STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_LTO_FLAGS     "Link with juce::juce_recommended_lto_flags. Available by default on all platforms."       ON)
option                  (STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_WARNING_FLAGS "Link with juce::juce_recommended_warning_flags. Available by default on all platforms."   ON)

if (NOT DEFINED STONEYDSP_BIQUADS_BUILD_FORMATS)

    set (STONEYDSP_BIQUADS_BUILD_FORMATS "")

    option                  (STONEYDSP_BIQUADS_BUILD_STANDALONE "" ON)
    cmake_dependent_option  (STONEYDSP_BIQUADS_BUILD_VST2       "" OFF  STONEYDSP_BIQUADS_VST2_SDK_PATH OFF)
    option                  (STONEYDSP_BIQUADS_BUILD_VST3       "" ON)
    cmake_dependent_option  (STONEYDSP_BIQUADS_BUILD_AAX        "" OFF  STONEYDSP_BIQUADS_AAX_SDK_PATH  OFF)
    cmake_dependent_option  (STONEYDSP_BIQUADS_BUILD_AU         "" ON   DARWIN                          ON)
    cmake_dependent_option  (STONEYDSP_BIQUADS_BUILD_AUV3       "" ON   DARWIN                          OFF)
    cmake_dependent_option  (STONEYDSP_BIQUADS_BUILD_LV2        "" OFF  UNIX                            OFF)

    if (STONEYDSP_BIQUADS_BUILD_STANDALONE)
        list (APPEND STONEYDSP_BIQUADS_BUILD_FORMATS "Standalone")
    endif ()

    if (STONEYDSP_BIQUADS_BUILD_VST2) # requires <STONEYDSP_BIQUADS_VST2_SDK_PATH>
        list (APPEND STONEYDSP_BIQUADS_BUILD_FORMATS "VST2")
    endif ()

    if (STONEYDSP_BIQUADS_BUILD_VST3)
        list (APPEND STONEYDSP_BIQUADS_BUILD_FORMATS "VST3")
    endif ()

    if (STONEYDSP_BIQUADS_BUILD_AAX) # requires <STONEYDSP_BIQUADS_AAX_SDK_PATH>
        list (APPEND STONEYDSP_BIQUADS_BUILD_FORMATS "AAX")
    endif ()

    if (STONEYDSP_BIQUADS_BUILD_AU) # requires <DARWIN:TRUE>
        list (APPEND STONEYDSP_BIQUADS_BUILD_FORMATS "AU")
    endif ()

    if (STONEYDSP_BIQUADS_BUILD_AU) # requires <DARWIN:TRUE>
        list (APPEND STONEYDSP_BIQUADS_BUILD_FORMATS "AUv3")
    endif ()

    if (STONEYDSP_BIQUADS_BUILD_LV2) # requires <UNIX:TRUE>
        list (APPEND STONEYDSP_BIQUADS_BUILD_FORMATS "lv2")
    endif ()

    set (STONEYDSP_BIQUADS_BUILD_FORMATS "${STONEYDSP_BIQUADS_BUILD_FORMATS}" CACHE STRING "Formats to build." FORCE)

endif () # STONEYDSP_BIQUADS_BUILD_FORMATS

cmake_dependent_option (STONEYDSP_BIQUADS_VST3_AUTO_MANIFEST "" ON STONEYDSP_BIQUADS_BUILD_VST3 OFF)

# These must be absolute paths. Maybe get_gilename_component on them...
if (DEFINED STONEYDSP_BIQUADS_AAX_SDK_PATH)
    set (STONEYDSP_BIQUADS_AAX_SDK_PATH "${STONEYDSP_BIQUADS_AAX_SDK_PATH}" CACHE PATH "" FORCE)
    juce_set_aax_sdk_path   ("${STONEYDSP_BIQUADS_AAX_SDK_PATH}")
endif ()
if (DEFINED STONEYDSP_BIQUADS_VST2_SDK_PATH)
    set (STONEYDSP_BIQUADS_VST2_SDK_PATH "${STONEYDSP_BIQUADS_VST2_SDK_PATH}" CACHE PATH "" FORCE)
    juce_set_vst2_sdk_path  ("${STONEYDSP_BIQUADS_VST2_SDK_PATH}")
endif ()
if (DEFINED STONEYDSP_BIQUADS_VST3_SDK_PATH)
    set (STONEYDSP_BIQUADS_VST3_SDK_PATH "${STONEYDSP_BIQUADS_VST3_SDK_PATH}" CACHE PATH "" FORCE)
    juce_set_vst3_sdk_path   ("${STONEYDSP_BIQUADS_VST3_SDK_PATH}")
endif ()
if (DEFINED STONEYDSP_BIQUADS_ARA_SDK_PATH)
    set (STONEYDSP_BIQUADS_ARA_SDK_PATH "${STONEYDSP_BIQUADS_ARA_SDK_PATH}" CACHE PATH "" FORCE)
    juce_set_ara_sdk_path  ("${STONEYDSP_BIQUADS_ARA_SDK_PATH}")
endif ()

#[=============================================================================[
#   Dependency: StoneyDSP::stoneydsp_core
#   Dependency: StoneyDSP::stoneydsp_audio
#]=============================================================================]

# find_package (StoneyDSP CONFIG REQUIRED)

add_subdirectory (Modules)

#[=============================================================================[
#   Configuration
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

set (STONEYDSP_BIQUADS_UNIT_FILES "")
list (APPEND STONEYDSP_BIQUADS_UNIT_FILES
    "Parameters"
    "Wrapper"
    "Editor"
    "Processor"
)

set (STONEYDSP_BIQUADS_UNIT_FILE_EXTENSIONS "")
list (APPEND STONEYDSP_BIQUADS_UNIT_FILE_EXTENSIONS
    "hpp"
    "cpp"
    "mm"
)

include (StoneyDSPBiquadsHelpers)
# stoneydsp_biquads_add_juce_object_library(juce_core)

stoneydsp_biquads_add_juce_object_library(juce_audio_basics)
stoneydsp_biquads_add_juce_object_library(juce_audio_devices)
stoneydsp_biquads_add_juce_object_library(juce_audio_formats)
stoneydsp_biquads_add_juce_object_library(juce_audio_plugin_client)
stoneydsp_biquads_add_juce_object_library(juce_audio_processors)
stoneydsp_biquads_add_juce_object_library(juce_audio_utils)
stoneydsp_biquads_add_juce_object_library(juce_core)
stoneydsp_biquads_add_juce_object_library(juce_data_structures)
stoneydsp_biquads_add_juce_object_library(juce_dsp)
stoneydsp_biquads_add_juce_object_library(juce_events)
stoneydsp_biquads_add_juce_object_library(juce_graphics)
stoneydsp_biquads_add_juce_object_library(juce_gui_basics)
stoneydsp_biquads_add_juce_object_library(juce_gui_extra)

#[=============================================================================[
#   target: StoneyDSP::Biquads::Parameters
#   target: StoneyDSP::Biquads::Wrapper
#   target: StoneyDSP::Biquads::Editor
#   target: StoneyDSP::Biquads::Processor
#]=============================================================================]

# The below custom function generates several libraries using some simplified
# arguments. The libraries generated may vary, but are generally as follows:
#
# <namespace>::<name>_INTERFACE - an INTERFACE library carrying the header and source files for this unit
# <namespace>::<name>_JUCE_DEPENDENCIES - an INTERFACE library carrying all of the JUCE requirements for this unit
# <namespace>::<name>_OBJECT - an OBJECT library representing the sum total of this compilation unit (for customary debugging/unit-testing stuff)
# <namespace>::<name> - an INTERFACE library representing the sum total of this unit (for linking to our plugin target)
#
# Other libs might get added, or these might get shuffled around internally, as
# I do battle with JUCE's unusual CMake implementation, which makes it
# impossible to call the regular CMake 'export()' and 'install()' commands on
# targets that consume their modules (and helpers).
#
# The idea is, we will seperate the headers/sources and JUCE their dependencies
# into logical targets, and make it possible to export/install our unit files
# without any stated linkaged requirements to JUCE; CMake's package config file
# helpers can then be used to write up the 'StoneyDSPBiquadsConfig.cmake' file
# which will in turn tell end-users' CMake about the JUCE requirements later on.
# We basically need to 'bluff' the export/install commands, and pass the JUCE
# requirements along - probably using a more convential configuration - for the
# config file to do the remainder of the work, post - export/install.

foreach (STONEYDSP_BIQUADS_UNIT_FILE IN LISTS STONEYDSP_BIQUADS_UNIT_FILES)

    stoneydsp_biquads_add_unit (${STONEYDSP_BIQUADS_UNIT_FILE}

        NAMESPACE StoneyDSP::Biquads
        VERSION "${STONEYDSP_BIQUADS_VERSION_MAJOR}.${STONEYDSP_BIQUADS_VERSION_MINOR}.${STONEYDSP_BIQUADS_VERSION_PATCH}"
        SOVERSION "${STONEYDSP_BIQUADS_VERSION_MAJOR}"

        SOURCE_INTERFACE    "Sources/${STONEYDSP_BIQUADS_UNIT_FILE}" # Where to find the source files for this unit
        BUILD_INTERFACE     "Sources/${STONEYDSP_BIQUADS_UNIT_FILE}" # Where to place the configured files for this unit
        INSTALL_INTERFACE   "include/StoneyDSP/Biquads/${STONEYDSP_BIQUADS_UNIT_FILE}" # Where to prefix the installation of this unit
    )

    list (APPEND STONEYDSP_BIQUADS_EXPORT_TARGETS ${STONEYDSP_BIQUADS_UNIT_FILE}_INTERFACE) # Keep a list of targets to pass to 'export/install()' later...

endforeach ()

# The targets created above, once known, can then be called on as regular
# CMake targets, with the properties of the library types described above.

# Here, we can specifiy every '#include <juce*/juce_*.h>' file we need in each
# unit, rather than relying a global JUCE header. The function wraps our
# libraries in such a way that no JUCE objects are duplicated during the build
# process, avoiding unneccessary build times and linker symbol duplication
# errors, but still permitting export/install to some degree.

target_link_libraries (Parameters_JUCE_DEPENDENCIES
    INTERFACE
        juce::juce_audio_basics # depends: juce_core
        juce::juce_audio_processors # depends: juce_gui_extra, juce_audio_basics
        juce::juce_core
        juce::juce_data_structures # depends: juce_events
        juce::juce_events # depends: juce_core
        juce::juce_graphics # depends: juce_events
        juce::juce_gui_basics # depends: juce_graphics, juce_data_structures
        juce::juce_gui_extra # depends: juce_gui_basics
)

target_link_libraries (Wrapper_JUCE_DEPENDENCIES
    INTERFACE
        juce::juce_audio_basics # depends: juce_core
        juce::juce_audio_formats # depends: juce_audio_basics
        juce::juce_audio_processors # depends: juce_gui_extra, juce_audio_basics
        juce::juce_core #
        juce::juce_data_structures # depends: juce_events
        juce::juce_dsp # depends: juce_audio_formats
        juce::juce_events # depends: juce_core
        juce::juce_graphics # depends: juce_events
        juce::juce_gui_basics # depends: juce_graphics, juce_data_structures
        juce::juce_gui_extra # depends: juce_gui_basics

        # StoneyDSP::stoneydsp_audio
        # StoneyDSP::stoneydsp_core
)

target_link_libraries (Editor_JUCE_DEPENDENCIES # This one may change...
    INTERFACE
        juce::juce_audio_basics # depends: juce_core
        juce::juce_audio_formats # depends: juce_audio_basics
        juce::juce_audio_processors # depends: juce_gui_extra, juce_audio_basics
        juce::juce_core
        juce::juce_data_structures # depends: juce_events
        juce::juce_dsp # depends: juce_audio_formats
        juce::juce_events # depends: juce_core
        juce::juce_graphics # depends: juce_events
        juce::juce_gui_basics # depends: juce_graphics, juce_data_structures
        juce::juce_gui_extra # depends: juce_gui_basics
)

target_link_libraries (Processor_JUCE_DEPENDENCIES
    INTERFACE
        juce::juce_audio_basics # depends: juce_core
        juce::juce_audio_formats # depends: juce_audio_basics
        juce::juce_audio_processors # depends: juce_gui_extra, juce_audio_basics
        juce::juce_core
        juce::juce_data_structures # depends: juce_events
        juce::juce_dsp # depends: juce_audio_formats
        juce::juce_events # depends: juce_core
        juce::juce_graphics # depends: juce_events
        juce::juce_gui_basics # depends: juce_graphics, juce_data_structures
        juce::juce_gui_extra # depends: juce_gui_basics
)

# Likewise, if one unit depends on another, we can specify that here. Since
# the INTERFACE targets are all safe to export/install, we may as well specify
# these inter-dendencies now, instead of passing this on to the config file,
# for downstream installers of this module.

target_link_libraries (Wrapper_INTERFACE
    INTERFACE
        StoneyDSP::Biquads::Parameters_INTERFACE
)

target_link_libraries (Processor_INTERFACE
    INTERFACE
        StoneyDSP::Biquads::Wrapper_INTERFACE
        StoneyDSP::Biquads::Parameters_INTERFACE
        StoneyDSP::Biquads::Editor_INTERFACE
)

# These objects can be globally shared to save compilation times! (WIP)

target_sources (Processor_OBJECT
    PRIVATE
        $<TARGET_OBJECTS:juce_audio_basics_OBJECT> # depends: juce_core
        $<TARGET_OBJECTS:juce_audio_formats_OBJECT> # depends: juce_audio_basics
        $<TARGET_OBJECTS:juce_audio_processors_OBJECT> # depends: juce_gui_extra, juce_audio_basics
        $<TARGET_OBJECTS:juce_core_OBJECT>
        $<TARGET_OBJECTS:juce_data_structures_OBJECT> # depends: juce_events
        $<TARGET_OBJECTS:juce_dsp_OBJECT> # depends: juce_audio_formats
        $<TARGET_OBJECTS:juce_events_OBJECT> # depends: juce_core
        $<TARGET_OBJECTS:juce_graphics_OBJECT> # depends: juce_events
        $<TARGET_OBJECTS:juce_gui_basics_OBJECT> # depends: juce_graphics, juce_data_structures
        $<TARGET_OBJECTS:juce_gui_extra_OBJECT> # depends: juce_gui_basics
)

#[=============================================================================[
#   target: StoneyDSP::Biquads
#]=============================================================================]

add_library (Biquads OBJECT)
add_library (StoneyDSP::Biquads::Biquads ALIAS Biquads)

target_compile_features(Biquads PUBLIC cxx_std_17)

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
    target_link_libraries (Biquads
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
    target_link_libraries (Biquads
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
    target_include_directories (Biquads
        PUBLIC
            $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources>
            $<INSTALL_INTERFACE:include/StoneyDSP/Biquads>
    )
else ()
    target_sources (Biquads
        PUBLIC
            FILE_SET biquads_AUDIO_PLUGIN_INTERFACE_HEADERS
            TYPE HEADERS
            BASE_DIRS
                $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources>
                $<INSTALL_INTERFACE:include/StoneyDSP>
            FILES
                $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources/Biquads.hpp>
                $<INSTALL_INTERFACE:include/StoneyDSP/Biquads.hpp>
    )
endif()

target_sources (Biquads
    PRIVATE
        $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources/Biquads.cpp>
        $<INSTALL_INTERFACE:include/StoneyDSP/Biquads.cpp>

        $<TARGET_OBJECTS:Parameters_OBJECT>
        $<TARGET_OBJECTS:Processor_OBJECT>
        $<TARGET_OBJECTS:Wrapper_OBJECT>
        $<TARGET_OBJECTS:Editor_OBJECT>
)
target_compile_definitions (Biquads
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        DONT_SET_USING_JUCE_NAMESPACE=1
)

#[=============================================================================[
#   target: StoneyDSP::Biquads_Audio_Plugin
#]=============================================================================]

option (STONEYDSP_BIQUADS_BUILD_AUDIO_PLUGIN "" OFF)
if (STONEYDSP_BIQUADS_BUILD_AUDIO_PLUGIN)

juce_add_plugin (Biquads_Audio_Plugin
    # VERSION ...                                                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                                                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...

    VST3_AUTO_MANIFEST FALSE # (see 'Helpers' section below)                    # May be either TRUE or FALSE (defaults to TRUE). When TRUE, a POST_BUILD step will be added to the VST3 target which will generate a moduleinfo.json file into the Resources subdirectory of the plugin bundle. This is normally desirable, but does require that the plugin can be successfully loaded immediately after building the VST3 target. If the plugin needs further processing before it can be loaded (e.g. custom signing), then set this option to FALSE to disable the automatic manifest generation. To generate the manifest at a later point in the build, use the juce_enable_vst3_manifest_step function. It is strongly recommended to generate a manifest for your plugin, as this allows compatible hosts to scan the plugin much more quickly, leading to an improved experience for users.
    COPY_PLUGIN_AFTER_BUILD FALSE # (see 'Helpers' section below)               # Should the plugin be installed to a default location after building?

    BUNDLE_ID "com.StoneyDSP.Biquads"

    MICROPHONE_PERMISSION_ENABLED FALSE
    CAMERA_PERMISSION_ENABLED FALSE
    BLUETOOTH_PERMISSION_ENABLED FALSE

    VST2_CATEGORY "kPlugCategEffect"
    VST3_CATEGORIES "Fx" "EQ"

    IS_MIDI_EFFECT FALSE                                                        # Is this plugin a MIDI effect?
    IS_SYNTH FALSE                                                              # Is this a synth or an effect?
    NEEDS_MIDI_INPUT FALSE                                                      # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT FALSE                                                     # Does the plugin need midi output?
    NEEDS_CURL FALSE
    NEEDS_WEB_BROWSER FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE                                           # Does the editor need keyboard focus?

    PLUGIN_MANUFACTURER_CODE Manu                                               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Jer3                                                            # A unique four-character plugin id with exactly one upper-case character
                                                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS ${STONEYDSP_BIQUADS_BUILD_FORMATS}                                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3

    PRODUCT_NAME "Biquads"                                                      # The name of the final executable, which can differ from the target name
    COMPANY_NAME "StoneyDSP"                                                    # Specify the name of the plugin's author
    COMPANY_WEBSITE "https://stoneydsp.com"
    COMPANY_EMAIL "nathanjhood@googlemail.com"
    COMPANY_COPYRIGHT "Copyright (c) 2024 - Nathan J. Hood <nathanjhood@googlemail.com>"

    # VST_COPY_DIR
    # VST3_COPY_DIR
    # AAX_COPY_DIR
    # AU_COPY_DIR
    # UNITY_COPY_DIR
)

target_sources (Biquads_Audio_Plugin
    PRIVATE

        # $<TARGET_OBJECTS:Parameters_OBJECT>
        # $<TARGET_OBJECTS:Processor_OBJECT>
        # $<TARGET_OBJECTS:Wrapper_OBJECT>
        # $<TARGET_OBJECTS:Editor_OBJECT>
        $<TARGET_OBJECTS:Biquads>
)

# target_link_libraries (Biquads_Audio_Plugin
#     PRIVATE
#     StoneyDSP::Biquads::Biquads
#         # StoneyDSP::Biquads::Parameters
#         # StoneyDSP::Biquads::Wrapper
#         # StoneyDSP::Biquads::Editor
#         # StoneyDSP::Biquads::Processor

#         StoneyDSP::Biquads::Parameters_INTERFACE
#         StoneyDSP::Biquads::Wrapper_INTERFACE
#         StoneyDSP::Biquads::Editor_INTERFACE
#         StoneyDSP::Biquads::Processor_INTERFACE
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


#[=============================================================================[
#   Helpers
#]=============================================================================]

if (STONEYDSP_BIQUADS_GENERATE_JUCE_HEADER)
    juce_generate_juce_header       (Biquads_Audio_Plugin)
endif ()

if (STONEYDSP_BIQUADS_VST3_AUTO_MANIFEST)
    juce_enable_vst3_manifest_step  (Biquads_Audio_Plugin)
endif ()

if (STONEYDSP_BIQUADS_COPY_PLUGIN_AFTER_BUILD)
    juce_enable_copy_plugin_step    (Biquads_Audio_Plugin)
endif ()

endif (STONEYDSP_BIQUADS_BUILD_AUDIO_PLUGIN)

#[=============================================================================[
#   CTest configuration
#]=============================================================================]

if (STONEYDSP_BIQUADS_BUILD_TESTS AND (STONEYDSP_BIQUADS_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    enable_testing ()
if (FALSE)
    # Pretend this is a plugin... we want to see all link/include lines resolved,
    # but only one copy of each object - *and* the required JUCE modules - being built.
    # I wrapped this away because I may have to tweak the implementation further to
    # make the components compatible with CMake's install interface, which requires
    # tricky workarounds because of JUCE's CMake implementation...
    # Later, this might become a useful framework for unit tests and stuff.
    add_library (unit_tests SHARED)
    add_library (StoneyDSP::Biquads::unit_tests ALIAS unit_tests)
    set_target_properties (unit_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
    target_link_libraries (unit_tests
        PUBLIC
            StoneyDSP::Biquads::Parameters
            StoneyDSP::Biquads::Wrapper
            StoneyDSP::Biquads::Editor
            StoneyDSP::Biquads::Processor
    )
endif ()
if (FALSE)
    add_library (interface_tests SHARED)
    add_library (StoneyDSP::Biquads::interface_tests ALIAS interface_tests)
    set_target_properties (interface_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
    target_link_libraries (interface_tests
        PUBLIC
            StoneyDSP::Biquads::Parameters_INTERFACE
            StoneyDSP::Biquads::Wrapper_INTERFACE
            StoneyDSP::Biquads::Editor_INTERFACE
            StoneyDSP::Biquads::Processor_INTERFACE
    )
endif ()
if (FALSE)
    add_library (object_tests SHARED)
    add_library (StoneyDSP::Biquads::object_tests ALIAS object_tests)
    set_target_properties (object_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
    target_sources (object_tests
        PRIVATE
            $<TARGET_OBJECTS:Parameters_OBJECT>
            $<TARGET_OBJECTS:Processor_OBJECT>
            $<TARGET_OBJECTS:Wrapper_OBJECT>
            $<TARGET_OBJECTS:Editor_OBJECT>
    )
endif ()

    # This way for Tracktion Pluginval tests against the built plugin...
    add_subdirectory (Testing)

endif ()

#[=============================================================================[
#   Export configuration
#]=============================================================================]

unset (STONEYDSP_BIQUADS_INCLUDEDIR)
set (STONEYDSP_BIQUADS_INCLUDEDIR
    $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts/Sources>
    $<INSTALL_INTERFACE:include/StoneyDSP/Biquads>
)

# export (
#     TARGETS ${STONEYDSP_BIQUADS_EXPORT_TARGETS}
#     NAMESPACE StoneyDSP::Biquads::
#     FILE StoneyDSPBiquadsTargets.cmake
# )

# if (CMAKE_MAJOR_VERSION GREATER_EQUAL 3 AND (CMAKE_MINOR_VERSION GREATER_EQUAL 23))
#     install (TARGETS ${STONEYDSP_BIQUADS_EXPORT_TARGETS}
#         EXPORT StoneyDSPBiquadsTargets
#         LIBRARY DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
#         ARCHIVE DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
#         RUNTIME DESTINATION  "${CMAKE_INSTALL_BINDIR}"
#         INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP/Biquads"

#         FILE_SET biquads_Wrapper_INTERFACE_HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP/Biquads/Wrapper"
#         FILE_SET biquads_Parameters_INTERFACE_HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP/Biquads/Parameters"
#         FILE_SET biquads_Editor_INTERFACE_HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP/Biquads/Editor"
#         FILE_SET biquads_Processor_INTERFACE_HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP/Biquads/Processor"
#     )
# else ()
#     install (TARGETS ${STONEYDSP_BIQUADS_EXPORT_TARGETS}
#         EXPORT StoneyDSPBiquadsTargets
#         LIBRARY DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
#         ARCHIVE DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
#         RUNTIME DESTINATION  "${CMAKE_INSTALL_BINDIR}"
#         INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP/Biquads"

#         PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP/Biquads" # hmmm... not right.
#     )
# endif ()

# install(
#   EXPORT StoneyDSPBiquadsTargets
#   FILE StoneyDSPBiquadsTargets.cmake
#   NAMESPACE StoneyDSP::Biquads::
#   DESTINATION "lib/cmake/StoneyDSP/Biquads"
#   EXPORT_LINK_INTERFACE_LIBRARIES
# )

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

configure_package_config_file(
    "${STONEYDSP_BIQUADS_SOURCE_DIR}/share/StoneyDSP/Biquads/StoneyDSPBiquadsConfig.cmake.in"
    "${STONEYDSP_BIQUADS_BINARY_DIR}/StoneyDSPBiquadsConfig.cmake"
    INSTALL_DESTINATION "lib/cmake/StoneyDSP/Biquads"
)

install (
    DIRECTORY "${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts"
    DESTINATION "Builds"
)

install (FILES
    "${STONEYDSP_BIQUADS_BINARY_DIR}/StoneyDSPBiquadsConfigVersion.cmake"
    "${STONEYDSP_BIQUADS_BINARY_DIR}/StoneyDSPBiquadsConfig.cmake"
    "${STONEYDSP_BIQUADS_SOURCE_DIR}/share/cmake/Modules/GetGitRevListCount.cmake"
    "${STONEYDSP_BIQUADS_SOURCE_DIR}/share/cmake/Modules/StoneyDSPBiquadsHelpers.cmake"
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

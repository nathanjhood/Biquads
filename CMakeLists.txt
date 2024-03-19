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

set (STONEYDSP_BIQUADS_TARGETS "")

#find_package (StoneyDSP CONFIG REQUIRED)

#[=============================================================================[
#   target: StoneyDSP::stoneydsp_core
#   target: StoneyDSP::stoneydsp_audio
#]=============================================================================]

add_subdirectory(modules)

set_directory_properties(PROPERTIES

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

#[=============================================================================[
#   target: StoneyDSP::Biquads_Audio_Plugin_Dependencies
#]=============================================================================]

# Note: All linkage to this target MUST be marked PRIVATE
add_library (Biquads_Audio_Plugin_Dependencies STATIC)
add_library (StoneyDSP::Biquads_Audio_Plugin_Dependencies ALIAS Biquads_Audio_Plugin_Dependencies)
target_compile_features(Biquads_Audio_Plugin_Dependencies PRIVATE cxx_std_17)

set (STONEYDSP_BIQUADS_STONEYDSP_MODULE_DEPENDENCIES "")
list (APPEND STONEYDSP_BIQUADS_STONEYDSP_MODULE_DEPENDENCIES
    stoneydsp_core
    stoneydsp_audio
    # stoneydsp_graphics
)
foreach(STONEYDSP_BIQUADS_STONEYDSP_MODULE_DEPENDENCY IN LISTS STONEYDSP_BIQUADS_STONEYDSP_MODULE_DEPENDENCIES)
    target_link_libraries (Biquads_Audio_Plugin_Dependencies
        PRIVATE
            StoneyDSP::${STONEYDSP_BIQUADS_STONEYDSP_MODULE_DEPENDENCY}
    )
endforeach()

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
    target_link_libraries (Biquads_Audio_Plugin_Dependencies
        PRIVATE
            juce::${STONEYDSP_BIQUADS_JUCE_MODULE_DEPENDENCY}
    )
endforeach()

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
    target_link_libraries (Biquads_Audio_Plugin_Dependencies
        PUBLIC
            juce::${STONEYDSP_BIQUADS_JUCE_FLAG}
    )
endforeach ()

target_compile_definitions (Biquads_Audio_Plugin_Dependencies
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        DONT_SET_USING_JUCE_NAMESPACE=1
)
target_compile_definitions (Biquads_Audio_Plugin_Dependencies
    INTERFACE
        $<TARGET_PROPERTY:Biquads_Audio_Plugin_Dependencies,COMPILE_DEFINITIONS>
)
target_include_directories(Biquads_Audio_Plugin_Dependencies
    INTERFACE
        $<TARGET_PROPERTY:Biquads_Audio_Plugin_Dependencies,INCLUDE_DIRECTORIES>
)
set_target_properties(Biquads_Audio_Plugin_Dependencies PROPERTIES

    POSITION_INDEPENDENT_CODE       TRUE
    VISIBILITY_INLINES_HIDDEN       TRUE

    C_VISBILITY_PRESET              hidden
    CXX_VISIBILITY_PRESET           hidden

    VERSION                         ${STONEYDSP_BIQUADS_VERSION_MAJOR}.${STONEYDSP_BIQUADS_VERSION_MINOR}.${STONEYDSP_BIQUADS_VERSION_PATCH}
    SOVERSION                       ${STONEYDSP_BIQUADS_VERSION_MAJOR}

    RUNTIME_OUTPUT_DIRECTORY        "${STONEYDSP_BIQUADS_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY        "${STONEYDSP_BIQUADS_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY        "${STONEYDSP_BIQUADS_BINARY_DIR}/lib"
    PDB_OUTPUT_DIRECTORY            "${STONEYDSP_BIQUADS_BINARY_DIR}/bin"

    RUNTIME_OUTPUT_DIRECTORY_DEBUG  "${STONEYDSP_BIQUADS_BINARY_DIR}/bin/Debug"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG  "${STONEYDSP_BIQUADS_BINARY_DIR}/lib/Debug"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG  "${STONEYDSP_BIQUADS_BINARY_DIR}/lib/Debug"
    PDB_OUTPUT_DIRECTORY_DEBUG      "${STONEYDSP_BIQUADS_BINARY_DIR}/bin/Debug"

    EXCLUDE_FROM_ALL                FALSE
)

list (APPEND STONEYDSP_BIQUADS_TARGETS Biquads_Audio_Plugin_Dependencies)

#[=============================================================================[
#   target: StoneyDSP::Biquads_Audio_Plugin_Interface
#]=============================================================================]

# Note: All linkage to this target MUST be marked INTERFACE
add_library (Biquads_Audio_Plugin_Interface INTERFACE)
add_library (StoneyDSP::Biquads_Audio_Plugin_Interface ALIAS Biquads_Audio_Plugin_Interface)

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
target_compile_features(Biquads_Audio_Plugin_Interface INTERFACE cxx_std_17)
if (CMAKE_MAJOR_VERSION GREATER_EQUAL 3 AND (CMAKE_MINOR_VERSION LESS 23))
    target_include_directories (Biquads_Audio_Plugin_Interface INTERFACE ${STONEYDSP_BIQUADS_INCLUDEDIR})
else ()
    target_sources (Biquads_Audio_Plugin_Interface
        INTERFACE
            FILE_SET biquads_AUDIO_PLUGIN_INTERFACE_HEADERS
            TYPE HEADERS
            BASE_DIRS ${STONEYDSP_BIQUADS_INCLUDEDIR}
            FILES
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
set_target_properties(Biquads_Audio_Plugin_Interface PROPERTIES

    POSITION_INDEPENDENT_CODE       TRUE
    VISIBILITY_INLINES_HIDDEN       TRUE

    C_VISBILITY_PRESET              hidden
    CXX_VISIBILITY_PRESET           hidden

    VERSION                         ${STONEYDSP_BIQUADS_VERSION_MAJOR}.${STONEYDSP_BIQUADS_VERSION_MINOR}.${STONEYDSP_BIQUADS_VERSION_PATCH}
    SOVERSION                       ${STONEYDSP_BIQUADS_VERSION_MAJOR}

    RUNTIME_OUTPUT_DIRECTORY        "${STONEYDSP_BIQUADS_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY        "${STONEYDSP_BIQUADS_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY        "${STONEYDSP_BIQUADS_BINARY_DIR}/lib"
    PDB_OUTPUT_DIRECTORY            "${STONEYDSP_BIQUADS_BINARY_DIR}/bin"

    RUNTIME_OUTPUT_DIRECTORY_DEBUG  "${STONEYDSP_BIQUADS_BINARY_DIR}/bin/Debug"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG  "${STONEYDSP_BIQUADS_BINARY_DIR}/lib/Debug"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG  "${STONEYDSP_BIQUADS_BINARY_DIR}/lib/Debug"
    PDB_OUTPUT_DIRECTORY_DEBUG      "${STONEYDSP_BIQUADS_BINARY_DIR}/bin/Debug"

    EXCLUDE_FROM_ALL                TRUE
)

list (APPEND STONEYDSP_BIQUADS_TARGETS Biquads_Audio_Plugin_Interface)

#[=============================================================================[
#   target: StoneyDSP::Biquads_Audio_Plugin_Library
#]=============================================================================]

add_library (Biquads_Audio_Plugin_Library STATIC)
add_library (StoneyDSP::Biquads_Audio_Plugin_Library ALIAS Biquads_Audio_Plugin_Library)
target_compile_features (Biquads_Audio_Plugin_Library PRIVATE cxx_std_17)
target_include_directories (Biquads_Audio_Plugin_Library
    PUBLIC
        ${STONEYDSP_BIQUADS_INCLUDEDIR}
)
target_sources (Biquads_Audio_Plugin_Library
    PRIVATE
        "src/${STONEYDSP_BIQUADS_UNIT_PARAMETERS}.cpp"
        "src/${STONEYDSP_BIQUADS_UNIT_WRAPPER}.cpp"
        "src/${STONEYDSP_BIQUADS_UNIT_PROCESSOR}.cpp"
        "src/${STONEYDSP_BIQUADS_UNIT_EDITOR}.cpp"
)
target_link_libraries (Biquads_Audio_Plugin_Library
    PRIVATE
        StoneyDSP::Biquads_Audio_Plugin_Dependencies # MUST be PRIVATE!
    INTERFACE
        StoneyDSP::Biquads_Audio_Plugin_Interface # MUST be INTERFACE!
)
set_target_properties (Biquads_Audio_Plugin_Library PROPERTIES

    POSITION_INDEPENDENT_CODE       TRUE
    VISIBILITY_INLINES_HIDDEN       TRUE

    C_VISBILITY_PRESET              hidden
    CXX_VISIBILITY_PRESET           hidden

    VERSION                         ${STONEYDSP_BIQUADS_VERSION_MAJOR}.${STONEYDSP_BIQUADS_VERSION_MINOR}.${STONEYDSP_BIQUADS_VERSION_PATCH}
    SOVERSION                       ${STONEYDSP_BIQUADS_VERSION_MAJOR}

    RUNTIME_OUTPUT_DIRECTORY        "${STONEYDSP_BIQUADS_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY        "${STONEYDSP_BIQUADS_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY        "${STONEYDSP_BIQUADS_BINARY_DIR}/lib"
    PDB_OUTPUT_DIRECTORY            "${STONEYDSP_BIQUADS_BINARY_DIR}/bin"

    RUNTIME_OUTPUT_DIRECTORY_DEBUG  "${STONEYDSP_BIQUADS_BINARY_DIR}/bin/Debug"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG  "${STONEYDSP_BIQUADS_BINARY_DIR}/lib/Debug"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG  "${STONEYDSP_BIQUADS_BINARY_DIR}/lib/Debug"
    PDB_OUTPUT_DIRECTORY_DEBUG      "${STONEYDSP_BIQUADS_BINARY_DIR}/bin/Debug"

    EXCLUDE_FROM_ALL TRUE
)

list (APPEND STONEYDSP_BIQUADS_TARGETS Biquads_Audio_Plugin_Library)

#[=============================================================================[
#   target: StoneyDSP::Biquads
#]=============================================================================]

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

if (CMAKE_MAJOR_VERSION GREATER_EQUAL 3 AND (CMAKE_MINOR_VERSION LESS 23))
    target_include_directories (Biquads_Audio_Plugin PUBLIC ${STONEYDSP_BIQUADS_INCLUDEDIR})
else ()
    target_sources (Biquads_Audio_Plugin
        PUBLIC
            FILE_SET biquads_AUDIO_PLUGIN_HEADERS
            TYPE HEADERS
            BASE_DIRS ${STONEYDSP_BIQUADS_INCLUDEDIR}
            FILES
                $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/include/${STONEYDSP_BIQUADS_UNIT}.hpp>
                $<INSTALL_INTERFACE:include/${STONEYDSP_BIQUADS_UNIT}.hpp>
    )
endif()
target_sources (Biquads_Audio_Plugin
    PRIVATE
        "src/${STONEYDSP_BIQUADS_UNIT}.cpp"
)
target_link_libraries (Biquads_Audio_Plugin
    PRIVATE
        StoneyDSP::Biquads_Audio_Plugin_Dependencies # MUST be PRIVATE!
        StoneyDSP::Biquads_Audio_Plugin_Library
    INTERFACE
        StoneyDSP::Biquads_Audio_Plugin_Interface # MUST be INTERFACE!
)

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
    add_subdirectory (tests)
endif ()

#[=============================================================================[
#   Export targets configuration
#]=============================================================================]

include(CMakePackageConfigHelpers)

# export (
#     TARGETS Biquads_Audio_Plugin # ${STONEYDSP_BIQUADS_TARGETS}
#     FILE share/cmake/StoneyDSPBiquadsTargets.cmake
#     NAMESPACE StoneyDSP::
# )

# export (
#     TARGETS ${STONEYDSP_BIQUADS_JUCE_MODULE_DEPENDENCIES}
#     FILE share/cmake/StoneyDSPBiquadsJuceModuleDependencyTargets.cmake
#     NAMESPACE juce::
# )

#[=============================================================================[
#   Install & CPack configuration
#]=============================================================================]

include(GNUInstallDirs)

write_basic_package_version_file(
    "${STONEYDSP_BIQUADS_BINARY_DIR}/share/StoneyDSP/Biquads/StoneyDSPBiquadsConfigVersion.cmake"
    VERSION ${STONEYDSP_BIQUADS_VERSION}
    COMPATIBILITY ExactVersion
)

set(STONEYDSP_BIQUADS_INSTALL_LIBDIR "lib/cmake/StoneyDSP/Biquads" CACHE STRING "The location, relative to the install prefix, where the StoneyDSP Biquads config file will be installed")

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
    "${STONEYDSP_BIQUADS_BINARY_DIR}/share/StoneyDSP/Biquads/StoneyDSPBiquadsConfig.cmake"
    INSTALL_DESTINATION "${STONEYDSP_BIQUADS_INSTALL_LIBDIR}"
)

# if (STONEYDSP_BIQUADS_TARGET_IS_INSTALLABLE)
#     if (CMAKE_MAJOR_VERSION GREATER_EQUAL 3 AND (CMAKE_MINOR_VERSION LESS 23))
#         install(
#             TARGETS ${STONEYDSP_BIQUADS_TARGETS} ${STONEYDSP_BIQUADS_JUCE_MODULE_DEPENDENCIES}
#             EXPORT StoneyDSPBiquadsTargets
#             LIBRARY DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
#             ARCHIVE DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
#             RUNTIME DESTINATION  "${CMAKE_INSTALL_BINDIR}"
#             INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP"
#             PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP"
#             # FILE_SET biquads_HEADERS "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP"
#         )
#     else ()
#         install(
#             TARGETS ${STONEYDSP_BIQUADS_TARGETS} ${STONEYDSP_BIQUADS_JUCE_MODULE_DEPENDENCIES}
#             EXPORT StoneyDSPBiquadsTargets
#             LIBRARY DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
#             ARCHIVE DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
#             RUNTIME DESTINATION  "${CMAKE_INSTALL_BINDIR}"
#             INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP"
#             #PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP"
#             FILE_SET biquads_AUDIO_PLUGIN_INTERFACE_HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StoneyDSP"
#         )
#     endif ()
#     install(
#         EXPORT StoneyDSPBiquadsTargets
#         FILE share/cmake/StoneyDSPBiquadsTargets.cmake
#         NAMESPACE StoneyDSP::
#         DESTINATION lib/cmake/StoneyDSP
#     )
# endif ()

install (
    DIRECTORY "${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_Audio_Plugin_artefacts"  DESTINATION "Builds")

install (FILES
    "${STONEYDSP_BIQUADS_BINARY_DIR}/share/StoneyDSP/Biquads/StoneyDSPBiquadsConfigVersion.cmake"
    "${STONEYDSP_BIQUADS_BINARY_DIR}/share/StoneyDSP/Biquads/StoneyDSPBiquadsConfig.cmake"
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
    DESTINATION "${STONEYDSP_BIQUADS_INSTALL_LIBDIR}"
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
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES ${STONEYDSP_BIQUADS_CPACK_IGNORE_LIST})
include(CPack)

#[=============================================================================[
#
#]=============================================================================]

message (STATUS "Configuring StoneyDSP Biquads v${STONEYDSP_BIQUADS_VERSION} - done.")

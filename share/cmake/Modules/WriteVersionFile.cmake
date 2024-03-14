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

include (GetGitRef)

if (NOT DEFINED STONEYDSP_GIT_COMMIT_REF)
    stoneydsp_get_git_ref ()
endif ()

#[=============================================================================[
Provides:

    STONEYDSP_VERSION_FILE

]=============================================================================]#
macro (stoneydsp_write_version_file)
    set(_stoneydsp_tweak "")
    set(_stoneydsp_tweak ${STONEYDSP_GIT_COMMIT_REF})
    if(DEFINED StoneyDSP_VERSION_TWEAK AND (NOT StoneyDSP_VERSION_TWEAK STREQUAL ""))
        set(_stoneydsp_tweak ${StoneyDSP_VERSION_TWEAK})
    endif()
    set(STONEYDSP_VERSION_FILE "${StoneyDSP_SOURCE_DIR}/VERSION")
    file(WRITE "${STONEYDSP_VERSION_FILE}.tmp" "${StoneyDSP_VERSION_MAJOR}.${StoneyDSP_VERSION_MINOR}.${StoneyDSP_VERSION_PATCH}.${_stoneydsp_tweak}\n")

    #Copy the file only if it has changed.
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${STONEYDSP_VERSION_FILE}.tmp" "${STONEYDSP_VERSION_FILE}")
    file(REMOVE "${STONEYDSP_VERSION_FILE}.tmp")
    set(STONEYDSP_VERSION_FILE "${STONEYDSP_VERSION_FILE}" CACHE INTERNAL "StoneyDSP current version file." FORCE)
    unset(_stoneydsp_tweak)
endmacro ()

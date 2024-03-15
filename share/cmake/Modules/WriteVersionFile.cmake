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
.
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

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

include(GetGitRevListCount)

if (NOT DEFINED STONEYDSP_BIQUADS_VERSION_TWEAK_NUMBER)
    get_git_rev_list_count()
endif()

#[=============================================================================[
Provides:

    STONEYDSP_BIQUADS_VERSION_FILE

]=============================================================================]#
macro (stoneydsp_biquads_write_version_file)
    set(_stoneydsp_biquads_version_tweak "")
    set(_stoneydsp_biquads_version_tweak ${_git_rev_list_count})
    if(DEFINED STONEYDSP_BIQUADS_VERSION_TWEAK AND (NOT STONEYDSP_BIQUADS_VERSION_TWEAK STREQUAL ""))
        set(_stoneydsp_biquads_version_tweak ${STONEYDSP_BIQUADS_VERSION_TWEAK})
    endif()
    set(STONEYDSP_BIQUADS_VERSION_FILE "${STONEYDSP_BIQUADS_SOURCE_DIR}/VERSION")
    file(WRITE "${STONEYDSP_BIQUADS_VERSION_FILE}.tmp" "${STONEYDSP_BIQUADS_VERSION_MAJOR}.${STONEYDSP_BIQUADS_VERSION_MINOR}.${STONEYDSP_BIQUADS_VERSION_PATCH}.${_stoneydsp_biquads_version_tweak}\n")

    #Copy the file only if it has changed.
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${STONEYDSP_BIQUADS_VERSION_FILE}.tmp" "${STONEYDSP_BIQUADS_VERSION_FILE}")
    file(REMOVE "${STONEYDSP_BIQUADS_VERSION_FILE}.tmp")
    set(STONEYDSP_BIQUADS_VERSION_FILE "${STONEYDSP_BIQUADS_VERSION_FILE}" CACHE INTERNAL "StoneyDSP Biquads current version file." FORCE)
    unset(_stoneydsp_biquads_version_tweak)
endmacro ()

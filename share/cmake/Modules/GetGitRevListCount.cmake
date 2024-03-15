#[=============================================================================[
    Fetches 'git rev-list --count --first-parent HEAD' for use in CMake projects.
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
macro(get_git_rev_list_count)
    set(_git_rev_list_count 0)
    find_package(Git QUIET)
    if(GIT_FOUND)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-list --count --first-parent HEAD
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE test_result
            OUTPUT_VARIABLE _git_rev_list_count
        )
        if(test_result EQUAL 0)
            # If our VCS found a revision number, pass it to cache
            string(STRIP "${_git_rev_list_count}" _git_rev_list_count)
            set(_git_rev_list_count ${_git_rev_list_count})
            message(DEBUG "get_git_rev_list_count ${git_rev_list_count}")
        endif()
    else()
        message(WARNING "Git not found. Version cannot be determined.")
    endif()
endmacro()

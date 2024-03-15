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

#[=============================================================================[
Provides:

    STONEYDSP_BIQUADS_GIT_COMMIT_REF

]=============================================================================]#
macro(stoneydsp_biquads_get_git_ref)
  find_package(Git REQUIRED)
  if(GIT_FOUND)
      execute_process(
          COMMAND ${GIT_EXECUTABLE} rev-parse --git-dir
          WORKING_DIRECTORY ${STONEYDSP_BIQUADS_SOURCE_DIR}
          RESULT_VARIABLE test_result
          OUTPUT_VARIABLE git_output
      )
      if(test_result EQUAL 0)
          # If we found a '/.git' at the project root, fetch the full path and
          # store it as 'git_dir'
          get_filename_component(git_dir ${git_output} ABSOLUTE BASE_DIR "${STONEYDSP_BIQUADS_SOURCE_DIR}")
          string(STRIP "${git_dir}" git_dir)
          execute_process(
              COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
              WORKING_DIRECTORY ${STONEYDSP_BIQUADS_SOURCE_DIR}
              RESULT_VARIABLE test_result
              OUTPUT_VARIABLE STONEYDSP_BIQUADS_GIT_COMMIT_REF
          )
          if(test_result EQUAL 0)
              # If our VCS found a revision number, pass it to cache
              string(STRIP "${STONEYDSP_BIQUADS_GIT_COMMIT_REF}" STONEYDSP_BIQUADS_GIT_COMMIT_REF)
              # message(STATUS "STONEYDSP_BIQUADS_GIT_COMMIT_REF = ${STONEYDSP_BIQUADS_GIT_COMMIT_REF}")
          endif()

          execute_process(
              COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref --symbolic-full-name HEAD
              WORKING_DIRECTORY ${STONEYDSP_BIQUADS_SOURCE_DIR}
              RESULT_VARIABLE test_result
              OUTPUT_VARIABLE git_remote
          )
          if(test_result EQUAL 0)
              string(REPLACE "/" ";" branch ${git_remote})
              list(GET branch 0 git_remote)

          else()
              set(git_remote "origin")
          endif()
          # message(STATUS "git_remote = ${git_remote}")

          execute_process(
              COMMAND ${GIT_EXECUTABLE} remote get-url origin #${remote}
              WORKING_DIRECTORY ${StoneyDSP_SOURCE_DIR}
              RESULT_VARIABLE test_result
              OUTPUT_VARIABLE git_url
          )
          if(test_result EQUAL 0)
              string(STRIP "${git_url}" git_url)
              string(REPLACE "\n" "" git_url ${git_url})
          else()
          endif()
      endif()

      # message(STATUS "git_url = ${git_url}")
      # message(STATUS "git_dir = ${git_dir}")
  else()
      message(WARNING "Git not found. Version cannot be determined.")
  endif()

endmacro()

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

#[=============================================================================[
Provides:

    STONEYDSP_GIT_COMMIT_REF

]=============================================================================]#
macro(stoneydsp_get_git_ref)
  find_package(Git REQUIRED)
  if(GIT_FOUND)
      execute_process(
          COMMAND ${GIT_EXECUTABLE} rev-parse --git-dir
          WORKING_DIRECTORY ${StoneyDSP_SOURCE_DIR}
          RESULT_VARIABLE test_result
          OUTPUT_VARIABLE git_output
      )
      if(test_result EQUAL 0)
          # If we found a '/.git' at the project root, fetch the full path and
          # store it as 'git_dir'
          get_filename_component(git_dir ${git_output} ABSOLUTE BASE_DIR "${StoneyDSP_SOURCE_DIR}")
          string(STRIP "${git_dir}" git_dir)
          execute_process(
              COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
              WORKING_DIRECTORY ${StoneyDSP_SOURCE_DIR}
              RESULT_VARIABLE test_result
              OUTPUT_VARIABLE STONEYDSP_GIT_COMMIT_REF
          )
          if(test_result EQUAL 0)
              # If our VCS found a revision number, pass it to cache
              string(STRIP "${STONEYDSP_GIT_COMMIT_REF}" STONEYDSP_GIT_COMMIT_REF)
              # message(STATUS "STONEYDSP_GIT_COMMIT_REF = ${STONEYDSP_GIT_COMMIT_REF}")
          endif()

          execute_process(
              COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref --symbolic-full-name HEAD
              WORKING_DIRECTORY ${StoneyDSP_SOURCE_DIR}
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

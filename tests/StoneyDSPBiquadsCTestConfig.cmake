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

if(STONEYDSP_BIQUADS_BUILD_TESTS)

    # VST3 tests with pluginval
    set(_pluginval_platform "Windows")
    set(_pluginval_binary "pluginval.exe")
    set(_pluginval_fail "if %ERRORLEVEL% neq 0 exit /b 1")
    if(UNIX)
        set(_pluginval_platform "Linux")
        set(_pluginval_binary "pluginval")
        set(_pluginval_fail "|| exit 1")
    endif()
    if(APPLE)
        set(_pluginval_platform "macOS")
        set(_pluginval_binary "pluginval.app/Contents/MacOS/pluginval")
        set(_pluginval_fail "|| exit 1")
    endif()

    message(STATUS "Checking for binary 'pluginval'")

    if(EXISTS "${STONEYDSP_BIQUADS_BINARY_DIR}/${_pluginval_binary}")
        message(STATUS "  Found ${_pluginval_binary} for ${_pluginval_platform} in binary dir")
    else()

        if(EXISTS "${STONEYDSP_BIQUADS_BINARY_DIR}/pluginval_${_pluginval_platform}.zip")
            message(STATUS "  Found pluginval_${_pluginval_platform}.zip: ${STONEYDSP_BIQUADS_BINARY_DIR}/pluginval_${_pluginval_platform}.zip")
        else()

            if(NOT CURL_FETCH_PLUGINVAL_BINARY)
                message(STATUS "Fetching pluginval_${_pluginval_platform}.zip (Because STONEYDSP_BIQUADS_BUILD_TESTS = TRUE)...")
                execute_process(
                    RESULT_VARIABLE CURL_FETCH_PLUGINVAL_BINARY
                    COMMAND curl "-LO" "https://github.com/Tracktion/pluginval/releases/download/v1.0.3/pluginval_${_pluginval_platform}.zip"
                    WORKING_DIRECTORY "${STONEYDSP_BIQUADS_BINARY_DIR}"
                )
                message(STATUS "  Fetched pluginval_${_pluginval_platform}.zip: ${STONEYDSP_BIQUADS_BINARY_DIR}/pluginval_${_pluginval_platform}.zip")
            endif()

        endif()

        if(NOT 7ZIP_UNPACK_PLUGINVAL_BINARY)
            message(STATUS "Unpacking pluginval_${_pluginval_platform}.zip: ${STONEYDSP_BIQUADS_BINARY_DIR}/${_pluginval_binary}...")
            execute_process(
                RESULT_VARIABLE 7ZIP_UNPACK_PLUGINVAL_BINARY
                COMMAND 7z "x" "-y" "pluginval_${_pluginval_platform}.zip"
                WORKING_DIRECTORY "${STONEYDSP_BIQUADS_BINARY_DIR}"
            )
            message(STATUS "  Unpacked pluginval_${_pluginval_platform}.zip: ${STONEYDSP_BIQUADS_BINARY_DIR}/${_pluginval_binary}")
        endif()

    endif()

    macro(_stoneydsp_biquads_test_audiopluginexample_pluginval_vst3_validation)
        add_test(
        NAME test_pluginval_vst3_validation
        COMMAND "${STONEYDSP_BIQUADS_BINARY_DIR}/${_pluginval_binary}" "--output-dir ${STONEYDSP_BIQUADS_BINARY_DIR}" "--strictness-level 10" "--verbose" "--validate" "${STONEYDSP_BIQUADS_BINARY_DIR}/Biquads_artefacts/${CMAKE_BUILD_TYPE}/VST3/Biquads.vst3/" "${_pluginval_fail}"
        WORKING_DIRECTORY "${STONEYDSP_BIQUADS_BINARY_DIR}"
        )
    endmacro()

    macro(_stoneydsp_biquads_test_audiopluginexample_pluginval_vst3_unit_tests)
        add_test(
            NAME test_pluginval_vst3_unit_tests
            COMMAND "${STONEYDSP_BIQUADS_BINARY_DIR}/${_pluginval_binary}" "--output-dir ${STONEYDSP_BIQUADS_BINARY_DIR}" "--strictness-level 10" "--verbose" "--run-tests" "${StoneyDSP_BINARY_DIR}/Biquads_artefacts/${CMAKE_BUILD_TYPE}/VST3/Biquads.vst3/" "${_pluginval_fail}"
            WORKING_DIRECTORY "${STONEYDSP_BIQUADS_BINARY_DIR}"
        )
    endmacro()

    _stoneydsp_biquads_test_audiopluginexample_pluginval_vst3_validation()
    # _stoneydsp_biquads_test_audiopluginexample_pluginval_vst3_unit_tests()

    unset(_pluginval_platform)
    unset(_pluginval_binary)
    unset(_pluginval_fail)

endif()

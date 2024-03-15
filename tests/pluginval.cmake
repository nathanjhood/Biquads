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

add_test(
    NAME test_pluginval_vst3
    COMMAND "${NODE_EXECUTABLE}" "./tests/demo/${arg}.js"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)
set_tests_properties(test_${arg}
    PROPERTIES PASS_REGULAR_EXPRESSION " passed."
)

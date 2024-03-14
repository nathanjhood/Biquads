add_test(
    NAME test_pluginval_vst3
    COMMAND "${NODE_EXECUTABLE}" "./tests/demo/${arg}.js"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)
set_tests_properties(test_${arg}
    PROPERTIES PASS_REGULAR_EXPRESSION " passed."
)

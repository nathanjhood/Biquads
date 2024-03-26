function (stoneydsp_biquads_add_unit name)

    # Avoid duplicate target names
    if (TARGET ${name})
        message (SEND_ERROR "'stoneydsp_biquads_add_unit()' given target '${name}' which is already exists. Please choose a unique name for this target.")
        return ()
    endif ()

    string (TOUPPER "${name}" name_uppercase)
    string (TOLOWER "${name}" name_lowercase)

    if (CMAKE_MAJOR_VERSION GREATER_EQUAL 3 AND (CMAKE_MINOR_VERSION GREATER_EQUAL 23))
        set (${name}_HAS_FILE_SET TRUE)
    else ()
        set (${name}_HAS_FILE_SET FALSE)
    endif ()

    # PARSE ARGS

    set (options "")
    set (one_value_keywords "")
    set (multi_value_keywords "")
    set (args "")

    list (APPEND options
        "EXCLUDE_FROM_ALL"
    )
    list (APPEND one_value_keywords
        # "PREFIX"
        "PATH"
        "ALIAS"
        "NAMESPACE"
        "VERSION"
        "SOVERSION"
        "SOURCE_INTERFACE"
        "BUILD_INTERFACE"
        "INSTALL_INTERFACE"
    )
    list (APPEND multi_value_keywords
        "CONFIGURATIONS"
        "INCLUDE_DIRECTORIES"
        "LINK_LIBRARIES"
        # "SOURCES"
        # "HEADERS"
    )
    list (APPEND args
        "${ARGN}"
    )

    cmake_parse_arguments (ARG "${options}" "${one_value_keywords}" "${multi_value_keywords}" "${args}")

    set (namespace      "${ARG_NAMESPACE}") # Else use something else, etc...

    set (source_dir     "${ARG_SOURCE_INTERFACE}")
    set (build_dir      "${ARG_BUILD_INTERFACE}")
    set (install_dir    "${ARG_INSTALL_INTERFACE}")

    # DEPENDENCIES ...

    add_library (${name}_JUCE_DEPENDENCIES INTERFACE)
    add_library (${namespace}::${name}_JUCE_DEPENDENCIES ALIAS ${name}_JUCE_DEPENDENCIES)

    target_compile_definitions (${name}_JUCE_DEPENDENCIES
        INTERFACE
            JUCE_WEB_BROWSER=0
            JUCE_USE_CURL=0
            JUCE_VST3_CAN_REPLACE_VST2=0
            DONT_SET_USING_JUCE_NAMESPACE=1
    )

    set_target_properties (${name}_JUCE_DEPENDENCIES
        PROPERTIES
        VERSION ${JUCE_VERSION}
    )

    # INTERFACE

    configure_file (
        "${source_dir}/${name}.hpp"
        "${build_dir}/${name}.hpp"
    )

    stoneydsp_biquads_add_unit_interface (${name}

        NAMESPACE StoneyDSP::Biquads

        BASE_DIRS
            $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/${build_dir}>
            $<INSTALL_INTERFACE:${install_dir}>
        FILES
            $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/${build_dir}/${name}.hpp>
            $<INSTALL_INTERFACE:${install_dir}/${name}.hpp>
    )

    # OBJECT

    configure_file (
        "${source_dir}/${name}.cpp"
        "${build_dir}/${name}.cpp"
    )

    stoneydsp_biquads_add_unit_object (${name}

        NAMESPACE StoneyDSP::Biquads

        INCLUDE_DIRECTORIES
            $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/${build_dir}>
            $<INSTALL_INTERFACE:${install_dir}>
        SOURCES
            $<BUILD_INTERFACE:${STONEYDSP_BIQUADS_BINARY_DIR}/${build_dir}/${name}.cpp>
            $<INSTALL_INTERFACE:${install_dir}/${name}.cpp>
    )

    # OBJCXX

    configure_file (
        "${source_dir}/${name}.mm"
        "${build_dir}/${name}.mm"
    )

    # LINK

    target_link_libraries (${name}_INTERFACE INTERFACE ${namespace}::${name}_JUCE_DEPENDENCIES)

    # target_link_libraries (${name}_JUCE_DEPENDENCIES INTERFACE ${namespace}::${name}_INTERFACE)

    target_link_libraries (${name}_OBJECT INTERFACE ${namespace}::${name}_INTERFACE)

    target_link_libraries (${name}_OBJECT INTERFACE ${namespace}::${name}_JUCE_DEPENDENCIES)


    source_group ("${name}"
        FILES
            "${build_dir}/${name}.hpp"
            "${build_dir}/${name}.cpp"
            "${build_dir}/${name}.mm"
    )

endfunction (stoneydsp_biquads_add_unit)

function (stoneydsp_biquads_add_unit_interface name)

    # Avoid duplicate target names
    if (TARGET ${name})
        message (SEND_ERROR "'stoneydsp_biquads_add_unit_interface()' given target '${name}' which is already exists. Please choose a unique name for this target.")
        return ()
    endif ()

    string (TOUPPER "${name}" name_uppercase)
    string (TOLOWER "${name}" name_lowercase)

    if (CMAKE_MAJOR_VERSION GREATER_EQUAL 3 AND (CMAKE_MINOR_VERSION GREATER_EQUAL 23))
        set (${name}_HAS_FILE_SET TRUE)
    else ()
        set (${name}_HAS_FILE_SET FALSE)
    endif ()

    # PARSE ARGS

    set (options "")
    set (one_value_keywords "")
    set (multi_value_keywords "")
    set (args "")

    list (APPEND options
        "EXCLUDE_FROM_ALL"
    )
    list (APPEND one_value_keywords
        "PATH"
        "ALIAS"
        "NAMESPACE"
        "VERSION"
        "SOVERSION"
    )
    list (APPEND multi_value_keywords
        "CONFIGURATIONS"
        "BASE_DIRS"
        "FILES"
    )
    list (APPEND args
        "${ARGN}"
    )

    cmake_parse_arguments (ARG "${options}" "${one_value_keywords}" "${multi_value_keywords}" "${args}")

    set (namespace "${ARG_NAMESPACE}") # Else use something else, etc...

    add_library (${name}_INTERFACE INTERFACE)
    add_library (${namespace}::${name}_INTERFACE ALIAS ${name}_INTERFACE)

    set_property(
        TARGET ${name}_INTERFACE
        PROPERTY "${name}_IS_STONEYDSP_BIQUADS_TARGET" TRUE # Custom property
    )

    set (${name}_INTERFACE_VERSION      "${ARG_VERSION}")
    set (${name}_INTERFACE_SOVERSION    "${ARG_SOVERSION}")

    # set (${name}_INTERFACE_INCLUDE_DIRECTORIES "")
    # list (APPEND ${name}_INTERFACE_INCLUDE_DIRECTORIES ${ARG_INCLUDE_DIRECTORIES})

    set (${name}_INTERFACE_HEADERS_BASE_DIRS "")
    list (APPEND ${name}_INTERFACE_HEADERS_BASE_DIRS ${ARG_BASE_DIRS})

    set (${name}_INTERFACE_HEADERS_FILES "")
    list (APPEND ${name}_INTERFACE_HEADERS_FILES ${ARG_FILES})

    # set (${name}_INTERFACE_SOURCES "")
    # list (APPEND ${name}_INTERFACE_SOURCES ${ARG_SOURCES})


    # set_target_properties (${name}_INTERFACE PROPERTIES
    #     INTERFACE_INCLUDE_DIRECTORIES   "${${name}_INTERFACE_INCLUDE_DIRECTORIES}"
    #     INTERFACE_SOURCES               "${${name}_INTERFACE_SOURCES}"
    # )

    if (${name}_HAS_FILE_SET)

        target_sources (${name}_INTERFACE
            INTERFACE
                FILE_SET "biquads_${name}_INTERFACE_HEADERS"
                TYPE "HEADERS"
                BASE_DIRS
                    ${${name}_INTERFACE_HEADERS_BASE_DIRS}
                FILES
                    ${${name}_INTERFACE_HEADERS_FILES}
        )

    else () # ${name}_HAS_FILE_SET

        set_property(TARGET ${name}_INTERFACE
            APPEND PROPERTY
                INTERFACE_INCLUDE_DIRECTORIES
                    ${${name}_INTERFACE_HEADERS_BASE_DIRS}
        )

    endif () # ${name}_HAS_FILE_SET

endfunction (stoneydsp_biquads_add_unit_interface)

function (stoneydsp_biquads_add_unit_object name)

    # Avoid duplicate target names
    if (TARGET ${name})
        message (SEND_ERROR "'stoneydsp_biquads_add_unit_object()' given target '${name}' which is already exists. Please choose a unique name for this target.")
        return ()
    endif ()

    # # Check that this is a StoneyDSP Biquads target
    # get_target_property(${name}_is_stoneydsp_biquads_target ${name} "${name}_IS_STONEYDSP_BIQUADS_TARGET")
    # if(NOT TARGET ${name} OR NOT ${name}_is_stoneydsp_biquads_target)
    #     message(SEND_ERROR "'stoneydsp_biquads_add_unit_interface()' called on '${name}' which was not created with 'stoneydsp_biquads_add_unit()...'")
    #     return()
    # endif()

    string (TOUPPER "${name}" name_uppercase)
    string (TOLOWER "${name}" name_lowercase)

    # PARSE ARGS

    set (options "")
    set (one_value_keywords "")
    set (multi_value_keywords "")
    set (args "")

    list (APPEND options
        "EXCLUDE_FROM_ALL"
    )
    list (APPEND one_value_keywords
        "PATH"
        "ALIAS"
        "NAMESPACE"
        "VERSION"
        "SOVERSION"
    )
    list (APPEND multi_value_keywords
        "CONFIGURATIONS"
        "SOURCES"
        "INCLUDE_DIRECTORIES"
    )
    list (APPEND args
        "${ARGN}"
    )

    cmake_parse_arguments (ARG "${options}" "${one_value_keywords}" "${multi_value_keywords}" "${args}")

    add_library (${name}_OBJECT OBJECT)
    add_library (${namespace}::${name}_OBJECT ALIAS ${name}_OBJECT)

    set_property(
        TARGET ${name}_OBJECT
        PROPERTY "${name}_IS_STONEYDSP_BIQUADS_TARGET" TRUE # Custom property
    )

    set (${name}_OBJECT_VERSION      "${ARG_VERSION}")
    set (${name}_OBJECT_SOVERSION    "${ARG_SOVERSION}")

    set (${name}_OBJECT_INCLUDE_DIRECTORIES "")
    list (APPEND ${name}_OBJECT_INCLUDE_DIRECTORIES ${ARG_INCLUDE_DIRECTORIES})

    set (${name}_OBJECT_SOURCES "")
    list (APPEND ${name}_OBJECT_SOURCES ${ARG_SOURCES})

    target_sources (${name}_OBJECT
        PRIVATE
            ${${name}_OBJECT_SOURCES}
    )

    target_include_directories (${name}_OBJECT
        PUBLIC
            ${${name}_OBJECT_INCLUDE_DIRECTORIES}
    )



endfunction (stoneydsp_biquads_add_unit_object)

function (stoneydsp_biquads_add_unit_sources name)

    # Check that this is a StoneyDSP Biquads target
    get_target_property(${name}_is_stoneydsp_biquads_target ${name} "${name}_IS_STONEYDSP_BIQUADS_TARGET")
    if(NOT TARGET ${name} OR NOT ${name}_is_stoneydsp_biquads_target)
        message(SEND_ERROR "'stoneydsp_biquads_add_unit_sources()' called on '${name}' which was not created with 'stoneydsp_biquads_add_unit()...'")
        return()
    endif()

endfunction (stoneydsp_biquads_add_unit_sources)







function (_stoneydsp_biquads_path         path)

    cmake_path(SET file NORMALIZE "${path}")

    set (file "${file}")

    # The variable must exist and it is expected to hold a single path.

    if (NOT EXISTS ${} OR (NOT IS_DIRECTORY ${}))
    endif ()

    cmake_path (GET "${file}" EXTENSION file_ext)
    cmake_path (GET "${file}" STEM      file_name)

    get_filename_component (file_name      "${file}" NAME) # File name without directory
    get_filename_component (file_abs_path  "${file}" ABSOLUTE) # Full path to file
    get_filename_component (file_ext       "${file}" EXT) # File name longest extension (.b.c from d/a.b.c)

    file     (RELATIVE_PATH file_rel_path  "${CMAKE_CURRENT_BINARY_DIR}" "${file_abs_path}") # /${file_name}

endfunction ()



function (_stoneydsp_biquads_validate_namespace name)

    set (options "")
    set (one_value_keywords "")
    set (multi_value_keywords "")
    set (args "")

    list (APPEND options
        ""
    )
    list (APPEND one_value_keywords
        "ALIAS"
        "NAMESPACE"
    )
    list (APPEND multi_value_keywords
        ""
    )
    list (APPEND args
        "${ARGN}"
    )

    cmake_parse_arguments (ARG "${options}" "${one_value_keywords}" "${multi_value_keywords}" "${args}")

    set (namespace_regex "[a-zA-Z_][a-zA-Z0-9_]*") # Generate the identifier for the resource library's namespace

    if (NOT DEFINED ARG_NAMESPACE)
        # Check that the library name is also a valid namespace
        if (NOT name MATCHES "${namespace_regex}")
            message (SEND_ERROR "Library name is not a valid namespace. Specify the NAMESPACE argument")
            return ()
        endif ()
        set (ARG_NAMESPACE "${name}")
    else ()
        if (NOT ARG_NAMESPACE MATCHES "${namespace_regex}")
            message (SEND_ERROR "NAMESPACE for ${name} is not a valid C++ namespace identifier (${ARG_NAMESPACE})")
            return ()
        endif ()
    endif ()

    # if (ARG_ALIAS)
    #     set (name_alt "${ARG_ALIAS}")
    # else ()
    #     set (name_alt "${name}")
    # endif ()

endfunction (_stoneydsp_biquads_validate_namespace)

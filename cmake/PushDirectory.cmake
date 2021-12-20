macro(_push_directory_preprocess_arguments)
    cmake_parse_arguments(
        PARSE_ARGV 0 "PUSH_DIRECTORY" "SOURCE_DIRECTORY_COMES_LAST"
        "TARGET_NAME;OUTPUT_NAME;SOURCE_DIRECTORY"
        "SOURCE_FILES;INCLUDED_DIRECTORIES;LINKED_LIBRARIES"
    )
    if(NOT PUSH_DIRECTORY_TARGET_NAME)
        message(
            FATAL_ERROR
                "${PUSH_DIRECTORY_FUNCTION_NAME} requires a TARGET_NAME argument"
        )
    endif()
    if(NOT PUSH_DIRECTORY_SOURCE_FILES)
        message(
            FATAL_ERROR
                "${PUSH_DIRECTORY_FUNCTION_NAME} requires a SOURCE_FILES argument"
        )
    endif()
    if(NOT PUSH_DIRECTORY_SOURCE_DIRECTORY)
        set(PUSH_DIRECTORY_SOURCE_DIRECTORY "src")
    endif()
    if(PUSH_DIRECTORY_SOURCE_DIRECTORY_COMES_LAST)
        list(
            TRANSFORM PUSH_DIRECTORY_SOURCE_FILES
            PREPEND
                "${PUSH_DIRECTORY_TARGET_NAME}/${PUSH_DIRECTORY_SOURCE_DIRECTORY}/"
        )
    else()
        list(
            TRANSFORM PUSH_DIRECTORY_SOURCE_FILES
            PREPEND
                "${PUSH_DIRECTORY_SOURCE_DIRECTORY}/${PUSH_DIRECTORY_TARGET_NAME}/"
        )
    endif()
endmacro()

macro(_push_directory_apply_to_target TARGET_NAME)
    target_include_directories(
        "${TARGET_NAME}" PUBLIC "${PUSH_DIRECTORY_INCLUDED_DIRECTORIES}"
    )
    foreach(LIBRARY "${PUSH_DIRECTORY_LINKED_LIBRARIES}")
        target_link_libraries("${TARGET_NAME}" PUBLIC "${LIBRARY}")
    endforeach()
    if(PUSH_DIRECTORY_OUTPUT_NAME)
        set_target_properties(
            "${TARGET_NAME}" PROPERTIES OUTPUT_NAME
                                        "${PUSH_DIRECTORY_OUTPUT_NAME}"
        )
    endif()
endmacro()

function(push_directory_executable)
    set(PUSH_DIRECTORY_FUNCTION_NAME "push_directory_executable")
    _push_directory_preprocess_arguments()
    add_executable(
        "${PUSH_DIRECTORY_TARGET_NAME}" "${PUSH_DIRECTORY_SOURCE_FILES}"
    )
    _push_directory_apply_to_target("${PUSH_DIRECTORY_TARGET_NAME}")
endfunction()

function(push_directory_library)
    set(PUSH_DIRECTORY_FUNCTION_NAME "push_directory_library")
    _push_directory_preprocess_arguments()
    add_library(
        "${PUSH_DIRECTORY_TARGET_NAME}" SHARED "${PUSH_DIRECTORY_SOURCE_FILES}"
    )
    _push_directory_apply_to_target("${PUSH_DIRECTORY_TARGET_NAME}")
endfunction()

cmake_minimum_required(VERSION 3.12...3.23)

project(
    monkey.c
    LANGUAGES C
    VERSION 0.1.0
    DESCRIPTION "Monkey in C"
    HOMEPAGE_URL "https://github.com/Phytolizer/monkey.c"
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")
include(DeclareModule)

declare_module(
    Embed
    KIND executable
    SOURCES Embed.c
)
add_custom_command(
    OUTPUT "${PROJECT_BINARY_DIR}/Embedded/Test.h"
           "${PROJECT_BINARY_DIR}/Embedded/Test.c"
    DEPENDS Embed
    COMMAND "${CMAKE_COMMAND}" -E make_directory
            "${PROJECT_BINARY_DIR}/Embedded"
    COMMAND "$<TARGET_FILE:Embed>" "${PROJECT_SOURCE_DIR}/CMakeLists.txt"
            "${PROJECT_BINARY_DIR}/Embedded/Test"
)
declare_module(
    Test
    KIND library
    ABSOLUTE_SOURCES "${PROJECT_BINARY_DIR}/Embedded/Test.h"
                     "${PROJECT_BINARY_DIR}/Embedded/Test.c"
)

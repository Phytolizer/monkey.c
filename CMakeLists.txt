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

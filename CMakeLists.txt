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
declare_module(
    SimpleString
    KIND library
    SOURCES String.c
)
declare_module(
    Monkey
    KIND library
    SOURCES Lexer.c Token.c
    DEPENDS SimpleString
)

declare_module(TestFramework KIND interface)
add_custom_command(
    OUTPUT "${PROJECT_BINARY_DIR}/Embedded/MonkeyTest/Input/Lexer/NextToken.h"
           "${PROJECT_BINARY_DIR}/Embedded/MonkeyTest/Input/Lexer/NextToken.c"
    DEPENDS "${PROJECT_SOURCE_DIR}/Modules/MonkeyTest/Input/Lexer/NextToken.txt"
            Embed
    COMMAND "${CMAKE_COMMAND}" -E make_directory
            "${PROJECT_BINARY_DIR}/Embedded/MonkeyTest/Input/Lexer"
    COMMAND
        "$<TARGET_FILE:Embed>"
        "${PROJECT_SOURCE_DIR}/Modules/MonkeyTest/Input/Lexer/NextToken.txt"
        "${PROJECT_BINARY_DIR}/Embedded/MonkeyTest/Input/Lexer/NextToken"
)
declare_module(
    MonkeyTest
    KIND executable
    ABSOLUTE_SOURCES
        "${PROJECT_BINARY_DIR}/Embedded/MonkeyTest/Input/Lexer/NextToken.c"
    INCLUDES "${PROJECT_BINARY_DIR}/Embedded"
    SOURCES Main.c Lexer.c
    DEPENDS Monkey TestFramework
)

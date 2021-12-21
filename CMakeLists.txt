cmake_minimum_required(VERSION 3.12...3.22)

project(
    "monkey.c"
    LANGUAGES "C"
    VERSION "0.1.0"
    DESCRIPTION "The Monkey programming language, implemented in pure C"
    HOMEPAGE_URL "https://github.com/Phytolizer/monkey.c"
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(PushDirectory)

push_directory_library(
    TARGET_NAME "sds"
    SOURCE_FILES "sds.c"
    INCLUDED_DIRECTORIES "inc"
)
push_directory_library(
    TARGET_NAME "vec"
    SOURCE_FILES "src/vec.c"
    INCLUDED_DIRECTORIES "inc"
)
push_directory_library(
    TARGET_NAME "libhash"
    OUTPUT_NAME "hash"
    SOURCE_FILES "fnv.c" "hash.c"
    INCLUDED_DIRECTORIES "inc"
)
push_directory_library(
    TARGET_NAME "libnonstd"
    OUTPUT_NAME "nonstd"
    SOURCE_FILES "strdup.c" "strpbrk.c" "strspn.c" "strtok.c"
    INCLUDED_DIRECTORIES "inc"
)
push_directory_library(
    TARGET_NAME "libmonkey"
    OUTPUT_NAME "monkey"
    SOURCE_FILES "ast.c" "lexer.c" "parser.c" "token.c" "object.c" "evaluator.c"
    LINKED_LIBRARIES "libhash" "libnonstd" "sds" "vec"
    INCLUDED_DIRECTORIES "inc"
)

push_directory_executable(
    TARGET_NAME "monkey_repl"
    OUTPUT_NAME "monkey_repl.exe"
    SOURCE_FILES "main.c" "repl.c"
    LINKED_LIBRARIES "libmonkey"
)
push_directory_executable(
    TARGET_NAME "monkey_test"
    OUTPUT_NAME "monkey_test.exe"
    SOURCE_FILES "test.c" "test_ast.c" "test_lexer.c" "test_parser.c"
                 "test_evaluator.c"
    LINKED_LIBRARIES "libmonkey"
)

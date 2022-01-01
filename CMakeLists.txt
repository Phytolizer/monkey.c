cmake_minimum_required(VERSION 3.12...3.23)

project(
  monkey.c
  LANGUAGES C
  VERSION 0.1.0
  DESCRIPTION "Monkey in C"
  HOMEPAGE_URL "https://github.com/Phytolizer/monkey.c"
)

function(transform_sources PROJ)
  cmake_parse_arguments(
    PARSE_ARGV 0 "TS" "" "KIND" "SOURCES;ABSOLUTE_SOURCES;LIBRARIES;INCLUDES"
  )
  list(TRANSFORM TS_SOURCES PREPEND "${PROJ}/source/")
  if(TS_KIND STREQUAL "interface")
    add_library(${PROJ} INTERFACE)
    set(TS_PRIVACY INTERFACE)
  else()
    cmake_language(
      CALL "add_${TS_KIND}" ${PROJ} ${TS_SOURCES} ${TS_ABSOLUTE_SOURCES}
    )
    set(TS_PRIVACY PUBLIC)
  endif()
  target_include_directories(
    ${PROJ} ${TS_PRIVACY} "${PROJ}/include" ${TS_INCLUDES}
  )
  target_link_libraries(${PROJ} ${TS_PRIVACY} ${TS_LIBRARIES})
endfunction()

transform_sources(
  argparse
  KIND library
  SOURCES argparse.c
)
transform_sources(
  nonstd
  KIND library
  SOURCES asprintf.c strdup.c strpbrk.c strspn.c strtok.c
)
transform_sources(
  test
  KIND interface
  LIBRARIES nonstd
)
transform_sources(
  vec
  KIND library
  SOURCES vec.c
)
transform_sources(
  span
  KIND library
  SOURCES span.c
)
transform_sources(
  string
  KIND library
  SOURCES string.c
  LIBRARIES vec span
)
transform_sources(
  monkey
  KIND library
  SOURCES lexer.c token.c
  LIBRARIES vec span string
)
transform_sources(
  embed
  KIND executable
  SOURCES main.c
  LIBRARIES argparse nonstd
)
add_custom_command(
  OUTPUT "${PROJECT_BINARY_DIR}/embedded/next_token_test.c"
         "${PROJECT_BINARY_DIR}/embedded/next_token_test.h"
  DEPENDS embed
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${PROJECT_BINARY_DIR}/embedded"
  COMMAND
    "$<TARGET_FILE:embed>"
    "${PROJECT_SOURCE_DIR}/monkey_test/input/next_token.test" -o
    "${PROJECT_BINARY_DIR}/embedded/next_token_test.c" -s "next_token_test"
)
transform_sources(
  monkey_test
  KIND executable
  SOURCES main.c test_lexer.c
  ABSOLUTE_SOURCES "${PROJECT_BINARY_DIR}/embedded/next_token_test.c"
  LIBRARIES monkey test
  INCLUDES "${PROJECT_BINARY_DIR}/embedded"
)

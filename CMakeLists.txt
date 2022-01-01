cmake_minimum_required(VERSION 3.12...3.23)

project(
  monkey.c
  LANGUAGES C
  VERSION 0.1.0
  DESCRIPTION "Monkey in C"
  HOMEPAGE_URL "https://github.com/Phytolizer/monkey.c"
)

function(transform_sources PROJ)
  cmake_parse_arguments(PARSE_ARGV 0 "TS" "" "" "SOURCES;LIBRARIES")
  list(TRANSFORM TS_SOURCES PREPEND "${PROJ}/source/")
  add_library(${PROJ} ${TS_SOURCES})
  target_include_directories(${PROJ} PUBLIC "${PROJ}/include")
  target_link_libraries(${PROJ} PUBLIC ${TS_LIBRARIES})
endfunction()

transform_sources(vec SOURCES vec.c)
transform_sources(string SOURCES string.c LIBRARIES vec)
transform_sources(monkey SOURCES token.c LIBRARIES vec string)

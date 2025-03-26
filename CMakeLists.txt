cmake_minimum_required(VERSION 3.20.0)
project(enki VERSION 0.1.0)

enable_language(CXX C)

include(${CMAKE_CURRENT_SOURCE_DIR}/enki_cmake_definitions.cmake)

file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

add_library(enki STATIC)

set_target_properties(enki PROPERTIES
  ${ENKI_COMMON_PROPERTIES}
)

target_sources(enki PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp
)

target_include_directories(enki PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(enki PRIVATE
  ${ENKI_COMMON_COMPILE_OPTIONS}
)

option(ENKI_ENABLE_TESTS "Compile & run tests" OFF)
if(ENKI_ENABLE_TESTS)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()
option(ENKI_ENABLE_EXAMPLES "Compile examples" OFF)
if(ENKI_ENABLE_EXAMPLES)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
endif()

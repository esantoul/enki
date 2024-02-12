cmake_minimum_required(VERSION 3.20.0)

Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

include(CTest)
# enable_testing()

set(ENKI_TESTS_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/base_engine_arithmetic_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/base_engine_array_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/base_engine_composite_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/base_engine_custom_type_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/base_engine_enum_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/base_engine_range_constructible_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/base_engine_requirements.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/base_engine_tuple_like_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/manager_composite_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/manager_custom_type_register.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/manager_requirements.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/unit_tests/any_byte_input_iter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/unit_tests/any_byte_output_iter.cpp
)

add_executable(enki_tests ${ENKI_TESTS_SOURCES})

target_include_directories(enki_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(enki_tests PRIVATE
  -Wall
  -Wextra
  -Wshadow
  -Wnon-virtual-dtor
  -pedantic

  -Wold-style-cast
  -Wcast-align
  -Wunused
  -Woverloaded-virtual
  -Wnull-dereference

  # -Weffc++

  -fsanitize=address
)

target_link_options(enki_tests PRIVATE
  -fsanitize=address
)

target_link_libraries(enki_tests PRIVATE
  enki
  Catch2::Catch2WithMain
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)
catch_discover_tests(enki_tests)

add_custom_target(enki_tests_run ALL ctest --output-on-failure)
add_dependencies(enki_tests_run enki_tests)

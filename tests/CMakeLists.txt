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

set(ENKI_REGRESSION_TESTS_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/arithmetic_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/array_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/custom_type_dynamic_register.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/enum_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/range_constructible_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/tuple_like_serdes.cpp
)

add_executable(enki_regression_tests ${ENKI_REGRESSION_TESTS_SOURCES})

target_include_directories(enki_regression_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(enki_regression_tests PRIVATE
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

target_link_options(enki_regression_tests PRIVATE
  -fsanitize=address
)

target_link_libraries(enki_regression_tests PRIVATE
  enki
  Catch2::Catch2WithMain
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)
catch_discover_tests(enki_regression_tests)

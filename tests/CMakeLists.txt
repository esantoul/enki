cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 20)  # So that Catch2 compiles properly

Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.7.1
)
FetchContent_MakeAvailable(Catch2)

add_executable(enki_tests)

set_target_properties(enki_tests PROPERTIES
  ${ENKI_COMMON_PROPERTIES}
)

target_sources(enki_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/unit_tests/bin_writer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/arithmetic_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/enum_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/array_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/tuple_like_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/optional_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/variant_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/range_constructible_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/map_types_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/custom_type_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/composite_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/conversion_serdes.cpp
)

target_include_directories(enki_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(enki_tests PRIVATE
  ${ENKI_COMMON_COMPILE_OPTIONS}
  # -fsanitize=address
)

target_link_options(enki_tests PRIVATE
  # -fsanitize=address
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

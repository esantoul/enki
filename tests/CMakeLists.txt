cmake_minimum_required(VERSION 3.20.0)

Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(enki_tests)

set_target_properties(enki_tests PROPERTIES
  ${ENKI_COMMON_PROPERTIES}
)

target_sources(enki_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/arithmetic_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/enum_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/array_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/tuple_like_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/range_constructible_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/map_types_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/custom_type_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/base_engine_requirements.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/manager_composite_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/manager_custom_type_register.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/manager_requirements.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/unit_tests/any_byte_input_iter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/unit_tests/any_byte_output_iter.cpp
)

target_include_directories(enki_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(enki_tests PRIVATE
  ${ENKI_COMMON_COMPILE_OPTIONS}

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

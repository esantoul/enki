cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 20)  # So that Catch2 compiles properly

# Find Catch2 package
if (NOT DEFINED ENV{CPM_SOURCE_CACHE})
  message(FATAL_ERROR "Evironment variable `CPM_SOURCE_CACHE` needs to be defined, see file `README.md` section `Compilation`")
endif()

# If CPM is already included, use what is available
if (DEFINED CPM_VERSION)
  include(${CPM_DIRECTORY}/CPM.cmake)
else()
  include(${ENKI_SOURCE_DIR}/cmake/CPM.cmake)
endif()

if ((NOT DEFINED $CACHE{CPM_PACKAGE_Catch2_VERSION}) OR 
    (NOT $CACHE{CPM_PACKAGE_Catch2_VERSION} EQUAL 3.8.1))
  # Catch2 3.8.1 tag commit is 2b60af89e23d28eefc081bc930831ee9d45ea58b
  CPMAddPackage(
    NAME Catch2
    GITHUB_REPOSITORY "catchorg/Catch2"
    VERSION 3.8.1
    GIT_TAG 2b60af89e23d28eefc081bc930831ee9d45ea58b
  )
endif()

# Include Catch2's CMake scripts
list(APPEND CMAKE_MODULE_PATH $CACHE{Catch2_SOURCE_DIR}/extras)

add_executable(enki_tests)

set_target_properties(enki_tests PROPERTIES
  ${ENKI_COMMON_PROPERTIES}
)

target_sources(enki_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/unit_tests/bin_writer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/unit_tests/ctad.cpp
  # Binary format tests
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/binary_arithmetic_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/binary_enum_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/binary_array_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/binary_tuple_like_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/binary_optional_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/binary_variant_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/binary_range_constructible_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/binary_map_types_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/binary_custom_type_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/binary_composite_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/binary_conversion_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/binary_forward_compat_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/binary_edge_cases_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/binary_error_handling_serdes.cpp
  # JSON format tests
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/json_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/json_variant_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/json_forward_compat_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/json_edge_cases_serdes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/json_error_handling_serdes.cpp
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

include(CTest)
include(Catch)
catch_discover_tests(enki_tests)

add_custom_target(enki_tests_run ALL ctest --output-on-failure)
add_dependencies(enki_tests_run enki_tests)

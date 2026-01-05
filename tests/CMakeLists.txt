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
  ${CMAKE_CURRENT_SOURCE_DIR}/regression_tests/policy_forward_compat_serdes.cpp
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

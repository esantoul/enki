cmake_minimum_required(VERSION 3.20.0)
project(enki VERSION 0.1.0)

# include(CTest)
# enable_testing()

set(CMAKE_CXX_STANDARD 20)

add_library(enki INTERFACE)
target_include_directories(enki INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

option(ENKI_ENABLE_TESTS "Compile & run tests" OFF)
if(ENKI_ENABLE_TESTS)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()


# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)

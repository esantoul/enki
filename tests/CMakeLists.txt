cmake_minimum_required(VERSION 3.20.0)

# include(CTest)
# enable_testing()

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

add_executable(enki_tests ${SOURCES})

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

  # -fsanitize=undefined
)

target_link_options(enki_tests PRIVATE
  # -fsanitize=undefined
)

target_link_libraries(enki_tests PRIVATE enki)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)

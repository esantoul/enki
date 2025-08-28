cmake_minimum_required(VERSION 3.20.0)
project(enki_examples VERSION 0.1.0)

add_executable(enki_example example.cpp)

set_target_properties(enki_example PROPERTIES
  ${ENKI_COMMON_PROPERTIES}
)

target_compile_options(enki_example PRIVATE
  ${ENKI_COMMON_COMPILE_OPTIONS}
)

target_link_libraries(enki_example PRIVATE enki)

cmake_minimum_required(VERSION 3.26)

project(manim_cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(MANIM_CPP_BUILD_TESTS "Build C++ tests" ON)
option(MANIM_CPP_ENABLE_STRICT_WARNINGS "Enable strict compiler warnings" ON)

include(CTest)
include(GNUInstallDirs)

add_subdirectory(src)
add_subdirectory(apps/manim_cpp_cli)
add_subdirectory(apps/manim_cpp_migrate)
add_subdirectory(apps/manim_cpp_extract_frames)
add_subdirectory(apps/manim_alias_manim)
add_subdirectory(apps/manim_alias_manimce)
add_subdirectory(example_scenes)

if(MANIM_CPP_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests_cpp)
endif()

install(DIRECTORY include/manim_cpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

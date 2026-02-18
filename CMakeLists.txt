cmake_minimum_required(VERSION 3.26)

project(manim_cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enforce rewrite toolchain floor from the C++ migration plan.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "13.0")
    message(FATAL_ERROR "manim-cpp requires GCC 13+ (found ${CMAKE_CXX_COMPILER_VERSION}).")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "17.0")
    message(FATAL_ERROR "manim-cpp requires Clang 17+ (found ${CMAKE_CXX_COMPILER_VERSION}).")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "17.0")
    message(FATAL_ERROR "manim-cpp requires AppleClang 17+ (found ${CMAKE_CXX_COMPILER_VERSION}).")
  endif()
elseif(MSVC)
  if(MSVC_VERSION LESS 1938)
    message(FATAL_ERROR "manim-cpp requires MSVC 19.38+ (found ${MSVC_VERSION}).")
  endif()
endif()

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

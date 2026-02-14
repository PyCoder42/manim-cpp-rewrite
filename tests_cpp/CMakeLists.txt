cmake_minimum_required(VERSION 3.26)

include(FetchContent)

find_package(GTest QUIET)
if(NOT GTest_FOUND)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif()

add_executable(
  manim_cpp_tests
  unit/test_scene_registry.cpp
  unit/test_config.cpp
  unit/test_math_core.cpp
  unit/test_eigen_adapter.cpp
  unit/test_triangulation.cpp
  unit/test_path_ops.cpp
  unit/test_graph_layout.cpp
  unit/test_scene_lifecycle.cpp
  unit/test_scene_random.cpp
  unit/test_animation_timeline.cpp
  unit/test_scene_file_writer.cpp
  cli/test_cli.cpp
  plugin/test_plugin_abi.cpp
  migrate/test_migrate.cpp
)

target_link_libraries(
  manim_cpp_tests
  PRIVATE
    manim_cpp_core
    GTest::gtest_main
)

target_compile_features(manim_cpp_tests PRIVATE cxx_std_23)

include(GoogleTest)
gtest_discover_tests(manim_cpp_tests)

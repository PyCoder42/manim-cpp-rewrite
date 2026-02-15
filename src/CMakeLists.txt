find_package(Eigen3 QUIET CONFIG)

add_library(
  manim_cpp_core
  manim_cpp/animation/animation.cpp
  manim_cpp/animation/basic_animations.cpp
  manim_cpp/animation/composition.cpp
  manim_cpp/camera/camera.cpp
  manim_cpp/cli/cli.cpp
  manim_cpp/config/config.cpp
  manim_cpp/math/core.cpp
  manim_cpp/math/eigen_adapter.cpp
  manim_cpp/math/graph_layout.cpp
  manim_cpp/math/isocurve.cpp
  manim_cpp/math/path_ops.cpp
  manim_cpp/math/triangulation.cpp
  manim_cpp/migrate/migrate.cpp
  manim_cpp/mobject/graph.cpp
  manim_cpp/mobject/geometry.cpp
  manim_cpp/mobject/mobject.cpp
  manim_cpp/mobject/value_tracker.cpp
  manim_cpp/plugin/loader.cpp
  manim_cpp/renderer/cairo_renderer.cpp
  manim_cpp/renderer/opengl_renderer.cpp
  manim_cpp/renderer/renderer.cpp
  manim_cpp/renderer/shader_paths.cpp
  manim_cpp/scene/media_format.cpp
  manim_cpp/scene/registry.cpp
  manim_cpp/scene/scene_file_writer.cpp
  manim_cpp/scene/scene.cpp
  manim_cpp/testing/npz_archive.cpp
)

target_include_directories(
  manim_cpp_core
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(manim_cpp_core PUBLIC cxx_std_23)
target_link_libraries(manim_cpp_core PUBLIC ${CMAKE_DL_LIBS})

if(Eigen3_FOUND)
  target_link_libraries(manim_cpp_core PUBLIC Eigen3::Eigen)
else()
  find_path(
    MANIM_CPP_EIGEN3_INCLUDE_DIR
    Eigen/Core
    PATH_SUFFIXES eigen3
    PATHS /opt/homebrew/include /usr/local/include /usr/include
  )
  if(NOT MANIM_CPP_EIGEN3_INCLUDE_DIR)
    message(FATAL_ERROR "Eigen3 headers not found. Install Eigen3 before building manim-cpp.")
  endif()
  target_include_directories(manim_cpp_core PUBLIC ${MANIM_CPP_EIGEN3_INCLUDE_DIR})
endif()

if(MANIM_CPP_ENABLE_STRICT_WARNINGS)
  if(MSVC)
    target_compile_options(manim_cpp_core PRIVATE /W4 /permissive-)
  else()
    target_compile_options(manim_cpp_core PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endif()

install(TARGETS manim_cpp_core)

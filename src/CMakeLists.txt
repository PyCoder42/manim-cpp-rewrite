add_library(
  manim_cpp_core
  manim_cpp/animation/animation.cpp
  manim_cpp/camera/camera.cpp
  manim_cpp/cli/cli.cpp
  manim_cpp/config/config.cpp
  manim_cpp/migrate/migrate.cpp
  manim_cpp/mobject/mobject.cpp
  manim_cpp/plugin/loader.cpp
  manim_cpp/renderer/renderer.cpp
  manim_cpp/scene/registry.cpp
  manim_cpp/scene/scene_file_writer.cpp
  manim_cpp/scene/scene.cpp
)

target_include_directories(
  manim_cpp_core
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(manim_cpp_core PUBLIC cxx_std_23)
target_link_libraries(manim_cpp_core PUBLIC ${CMAKE_DL_LIBS})

if(MANIM_CPP_ENABLE_STRICT_WARNINGS)
  if(MSVC)
    target_compile_options(manim_cpp_core PRIVATE /W4 /permissive-)
  else()
    target_compile_options(manim_cpp_core PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endif()

install(TARGETS manim_cpp_core)

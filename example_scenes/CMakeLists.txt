add_library(
  manim_cpp_example_scenes
  OBJECT
  advanced_tex_fonts.cpp
  basic.cpp
  customtex.cpp
)

target_include_directories(
  manim_cpp_example_scenes
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(manim_cpp_example_scenes PRIVATE cxx_std_23)
target_link_libraries(manim_cpp_example_scenes PRIVATE manim_cpp_core)

if(MANIM_CPP_ENABLE_STRICT_WARNINGS)
  if(MSVC)
    target_compile_options(manim_cpp_example_scenes PRIVATE /W4 /permissive-)
  else()
    target_compile_options(manim_cpp_example_scenes PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endif()

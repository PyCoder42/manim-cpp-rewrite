#include "manim_cpp/renderer/shader_paths.hpp"

namespace manim_cpp::renderer {

std::filesystem::path default_shader_root() {
  return std::filesystem::path("src/manim_cpp/renderer/shaders");
}

}  // namespace manim_cpp::renderer

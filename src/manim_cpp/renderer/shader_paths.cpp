#include "manim_cpp/renderer/shader_paths.hpp"

#include <cstdlib>
#include <string>

namespace manim_cpp::renderer {

std::filesystem::path default_shader_root() {
  const char* override_root = std::getenv("MANIM_CPP_SHADER_ROOT");
  if (override_root != nullptr) {
    const std::string override_value = override_root;
    if (!override_value.empty()) {
      return std::filesystem::path(override_value);
    }
  }
  return std::filesystem::path("src/manim_cpp/renderer/shaders");
}

}  // namespace manim_cpp::renderer

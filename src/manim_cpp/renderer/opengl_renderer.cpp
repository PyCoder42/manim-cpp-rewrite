#include "manim_cpp/renderer/opengl_renderer.hpp"

#include <utility>

#include "manim_cpp/renderer/shader_paths.hpp"

namespace manim_cpp::renderer {

OpenGLRenderer::OpenGLRenderer() : shader_root_(default_shader_root()) {}

std::filesystem::path OpenGLRenderer::shader_root() const { return shader_root_; }

void OpenGLRenderer::set_shader_root(std::filesystem::path shader_root) {
  if (shader_root.empty()) {
    shader_root_ = default_shader_root();
    return;
  }
  shader_root_ = std::move(shader_root);
}

std::filesystem::path OpenGLRenderer::shader_path(
    const std::string& program_name,
    const std::string& stage) const {
  return shader_root_ / (program_name + "." + stage);
}

}  // namespace manim_cpp::renderer

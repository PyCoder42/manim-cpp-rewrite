#include "manim_cpp/renderer/opengl_renderer.hpp"

#include <iomanip>
#include <sstream>
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

std::string OpenGLRenderer::frame_file_name(const std::string& scene_name,
                                            const std::size_t frame_index) const {
  std::ostringstream stream;
  stream << scene_name << "_" << std::setfill('0') << std::setw(6) << frame_index
         << ".png";
  return stream.str();
}

bool OpenGLRenderer::should_render_for_signature(
    const std::string& frame_signature) {
  if (frame_signature.empty()) {
    return true;
  }
  return seen_frame_signatures_.insert(frame_signature).second;
}

void OpenGLRenderer::reset_frame_cache() { seen_frame_signatures_.clear(); }

}  // namespace manim_cpp::renderer

#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <unordered_set>

#include "manim_cpp/renderer/renderer.hpp"

namespace manim_cpp::renderer {

class OpenGLRenderer final : public Renderer {
 public:
  OpenGLRenderer();

  RendererType type() const override { return RendererType::kOpenGL; }
  std::string name() const override { return "opengl"; }

  std::filesystem::path shader_root() const;
  void set_shader_root(std::filesystem::path shader_root);
  std::filesystem::path shader_path(const std::string& program_name,
                                    const std::string& stage) const;
  std::string frame_file_name(const std::string& scene_name,
                              std::size_t frame_index) const;
  bool should_render_for_signature(const std::string& frame_signature);
  void reset_frame_cache();

 private:
  std::filesystem::path shader_root_;
  std::unordered_set<std::string> seen_frame_signatures_;
};

}  // namespace manim_cpp::renderer

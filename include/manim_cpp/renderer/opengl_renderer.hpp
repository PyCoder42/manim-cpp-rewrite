#pragma once

#include <filesystem>
#include <string>

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

 private:
  std::filesystem::path shader_root_;
};

}  // namespace manim_cpp::renderer

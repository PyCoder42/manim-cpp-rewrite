#pragma once

#include <cstddef>
#include <string>
#include <unordered_set>

#include "manim_cpp/renderer/renderer.hpp"

namespace manim_cpp::renderer {

class CairoRenderer final : public Renderer {
 public:
  RendererType type() const override { return RendererType::kCairo; }
  std::string name() const override { return "cairo"; }

  std::string frame_file_name(const std::string& scene_name,
                              std::size_t frame_index) const;
  bool should_render_for_signature(const std::string& frame_signature);
  void reset_frame_cache();

 private:
  std::unordered_set<std::string> seen_frame_signatures_;
};

}  // namespace manim_cpp::renderer

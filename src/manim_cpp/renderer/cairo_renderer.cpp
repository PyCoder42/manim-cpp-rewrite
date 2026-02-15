#include "manim_cpp/renderer/cairo_renderer.hpp"

#include <iomanip>
#include <sstream>

namespace manim_cpp::renderer {

std::string CairoRenderer::frame_file_name(const std::string& scene_name,
                                           const std::size_t frame_index) const {
  std::ostringstream stream;
  stream << scene_name << "_" << std::setfill('0') << std::setw(6) << frame_index
         << ".png";
  return stream.str();
}

bool CairoRenderer::should_render_for_signature(
    const std::string& frame_signature) {
  if (frame_signature.empty()) {
    return true;
  }
  return seen_frame_signatures_.insert(frame_signature).second;
}

void CairoRenderer::reset_frame_cache() { seen_frame_signatures_.clear(); }

}  // namespace manim_cpp::renderer

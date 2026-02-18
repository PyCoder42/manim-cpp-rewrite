#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_map>

#include <gtest/gtest.h>

#include "manim_cpp/cli/cli.hpp"
#include "manim_cpp/renderer/cairo_renderer.hpp"
#include "manim_cpp/renderer/opengl_renderer.hpp"
#include "manim_cpp/scene/registry.hpp"
#include "manim_cpp/scene/scene.hpp"

namespace {

class CliRenderBitwiseScene : public manim_cpp::scene::Scene {
 public:
  std::string scene_name() const override { return "CliRenderBitwiseScene"; }
  void construct() override { wait(0.5); }
};

MANIM_REGISTER_SCENE(CliRenderBitwiseScene);

class ScopedCurrentPath {
 public:
  explicit ScopedCurrentPath(const std::filesystem::path& next_path)
      : previous_(std::filesystem::current_path()) {
    std::filesystem::current_path(next_path);
  }

  ~ScopedCurrentPath() { std::filesystem::current_path(previous_); }

 private:
  std::filesystem::path previous_;
};

std::filesystem::path find_repo_root() {
  auto probe = std::filesystem::current_path();
  for (int depth = 0; depth < 12; ++depth) {
    if (std::filesystem::exists(
            probe / "tests_cpp" / "render_regression" / "test_control_data_npz.cpp")) {
      return probe;
    }
    if (!probe.has_parent_path()) {
      break;
    }
    probe = probe.parent_path();
  }
  return {};
}

std::string platform_id() {
#ifdef _WIN32
  return "windows";
#elif __APPLE__
  return "macos";
#else
  return "linux";
#endif
}

std::string fnv1a_64_hex(const std::filesystem::path& path) {
  std::ifstream input(path, std::ios::binary);
  if (!input.is_open()) {
    return {};
  }

  constexpr std::uint64_t kOffsetBasis = 14695981039346656037ull;
  constexpr std::uint64_t kPrime = 1099511628211ull;
  std::uint64_t hash = kOffsetBasis;

  char ch = 0;
  while (input.get(ch)) {
    hash ^= static_cast<std::uint8_t>(ch);
    hash *= kPrime;
  }

  std::ostringstream stream;
  stream << std::hex << std::setfill('0') << std::setw(16) << hash;
  return stream.str();
}

std::unordered_map<std::string, std::string> load_baseline_map(
    const std::filesystem::path& baseline_path) {
  std::unordered_map<std::string, std::string> values;
  std::ifstream input(baseline_path);
  if (!input.is_open()) {
    return values;
  }

  std::string line;
  while (std::getline(input, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }
    const auto separator = line.find('=');
    if (separator == std::string::npos) {
      continue;
    }
    const std::string key = line.substr(0, separator);
    const std::string value = line.substr(separator + 1);
    if (!key.empty() && !value.empty()) {
      values[key] = value;
    }
  }
  return values;
}

std::string dump_map(const std::unordered_map<std::string, std::string>& values) {
  std::ostringstream stream;
  for (const auto& [key, value] : values) {
    stream << key << "=" << value << "\n";
  }
  return stream.str();
}

void run_bitwise_parity_case(const std::filesystem::path& repo_root,
                             const std::string& renderer_name) {
  const auto temp_root = std::filesystem::temp_directory_path() /
                         ("manim_cpp_bitwise_render_" + renderer_name);
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  std::ofstream cfg(temp_root / "manim.cfg");
  cfg << "[CLI]\n";
  cfg << "media_dir = ./media\n";
  cfg << "video_dir = {media_dir}/videos/{module_name}/{quality}\n";
  cfg << "images_dir = {media_dir}/images/{module_name}\n";
  cfg << "partial_movie_dir = {video_dir}/partial_movie_files/{scene_name}\n";
  cfg << "pixel_width = 640\n";
  cfg << "pixel_height = 360\n";
  cfg << "frame_rate = 4\n";
  cfg.close();

  const auto input_name = "bitwise_scene_" + renderer_name + ".cpp";
  std::ofstream input_scene(temp_root / input_name);
  input_scene << "// placeholder\n";
  input_scene.close();

  {
    ScopedCurrentPath scoped_path(temp_root);
    const std::array<std::string, 7> args_storage = {
        "manim-cpp",   "render",      input_name,   "--scene",
        "CliRenderBitwiseScene", "--renderer", renderer_name,
    };
    const std::array<const char*, 7> args = {
        args_storage[0].c_str(), args_storage[1].c_str(), args_storage[2].c_str(),
        args_storage[3].c_str(), args_storage[4].c_str(), args_storage[5].c_str(),
        args_storage[6].c_str(),
    };
    ASSERT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 0);
  }

  const std::string module_name =
      std::filesystem::path(input_name).stem().string();
  const auto images_root = temp_root / "media" / "images" / module_name;
  const auto video_root = temp_root / "media" / "videos" / module_name / "360p4";
  const auto media_path = video_root / "CliRenderBitwiseScene.mp4";
  ASSERT_TRUE(std::filesystem::exists(media_path));

  std::string frame1_name;
  std::string frame2_name;
  if (renderer_name == "cairo") {
    manim_cpp::renderer::CairoRenderer renderer;
    frame1_name = renderer.frame_file_name("CliRenderBitwiseScene", 1);
    frame2_name = renderer.frame_file_name("CliRenderBitwiseScene", 2);
  } else {
    manim_cpp::renderer::OpenGLRenderer renderer;
    frame1_name = renderer.frame_file_name("CliRenderBitwiseScene", 1);
    frame2_name = renderer.frame_file_name("CliRenderBitwiseScene", 2);
  }

  const auto frame1_path = images_root / frame1_name;
  const auto frame2_path = images_root / frame2_name;
  ASSERT_TRUE(std::filesystem::exists(frame1_path));
  ASSERT_TRUE(std::filesystem::exists(frame2_path));

  std::unordered_map<std::string, std::string> actual = {
      {frame1_name, fnv1a_64_hex(frame1_path)},
      {frame2_name, fnv1a_64_hex(frame2_path)},
      {media_path.filename().string(), fnv1a_64_hex(media_path)},
  };

  const auto baseline_path = repo_root / "tests_cpp" / "render_regression" /
                             "baselines" / platform_id() /
                             ("render_" + renderer_name + ".txt");
  ASSERT_TRUE(std::filesystem::exists(baseline_path))
      << "Missing baseline file: " << baseline_path;
  const auto expected = load_baseline_map(baseline_path);
  ASSERT_FALSE(expected.empty()) << "Baseline file is empty: " << baseline_path;
  ASSERT_EQ(actual.size(), expected.size())
      << "Actual:\n"
      << dump_map(actual) << "Expected:\n"
      << dump_map(expected);

  for (const auto& [name, expected_hash] : expected) {
    const auto it = actual.find(name);
    ASSERT_NE(it, actual.end()) << "Missing artifact hash for " << name;
    EXPECT_EQ(it->second, expected_hash) << "Artifact hash mismatch for " << name;
  }

  std::filesystem::remove_all(temp_root);
}

}  // namespace

TEST(RenderBitwiseParity, CairoAndOpenGLMatchPlatformBaselines) {
  const auto repo_root = find_repo_root();
  ASSERT_FALSE(repo_root.empty());

  run_bitwise_parity_case(repo_root, "cairo");
  run_bitwise_parity_case(repo_root, "opengl");
}

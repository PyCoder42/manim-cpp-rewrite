#include <cstdlib>
#include <filesystem>

#include <gtest/gtest.h>

#include "manim_cpp/renderer/shader_paths.hpp"

namespace {

void set_shader_root_env(const std::string& value) {
#ifdef _WIN32
  _putenv_s("MANIM_CPP_SHADER_ROOT", value.c_str());
#else
  setenv("MANIM_CPP_SHADER_ROOT", value.c_str(), 1);
#endif
}

void unset_shader_root_env() {
#ifdef _WIN32
  _putenv_s("MANIM_CPP_SHADER_ROOT", "");
#else
  unsetenv("MANIM_CPP_SHADER_ROOT");
#endif
}

}  // namespace

TEST(ShaderPaths, UsesRepositoryDefaultWhenEnvironmentOverrideIsUnset) {
  unset_shader_root_env();

  EXPECT_EQ(manim_cpp::renderer::default_shader_root(),
            std::filesystem::path("src/manim_cpp/renderer/shaders"));
}

TEST(ShaderPaths, UsesEnvironmentOverrideWhenProvided) {
  const auto override_root =
      std::filesystem::temp_directory_path() / "manim_cpp_shader_override";
  std::filesystem::remove_all(override_root);
  std::filesystem::create_directories(override_root);
  set_shader_root_env(override_root.string());

  EXPECT_EQ(manim_cpp::renderer::default_shader_root(), override_root);

  unset_shader_root_env();
  std::filesystem::remove_all(override_root);
}

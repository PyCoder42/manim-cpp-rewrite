#include <filesystem>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

#include "manim_cpp/plugin/loader.hpp"

namespace {

#ifdef _WIN32
constexpr const char* kLibraryExtension = ".dll";
#elif __APPLE__
constexpr const char* kLibraryExtension = ".dylib";
#else
constexpr const char* kLibraryExtension = ".so";
#endif

struct HostCallbackState {
  int log_level = -1;
  std::string log_message;
  std::string scene_name;
  std::string symbol_name;
  int register_result = 0;
};

HostCallbackState* g_host_callback_state = nullptr;

void capture_log(int level, const char* message) {
  if (g_host_callback_state == nullptr) {
    return;
  }
  g_host_callback_state->log_level = level;
  g_host_callback_state->log_message = message != nullptr ? message : "";
}

int capture_registration(const char* scene_name, const char* symbol_name) {
  if (g_host_callback_state == nullptr) {
    return 1;
  }
  g_host_callback_state->scene_name = scene_name != nullptr ? scene_name : "";
  g_host_callback_state->symbol_name = symbol_name != nullptr ? symbol_name : "";
  return g_host_callback_state->register_result;
}

manim_plugin_host_api_v1 make_host_api(const uint32_t abi_version) {
  return manim_plugin_host_api_v1{
      .abi_version = abi_version,
      .log_message = nullptr,
      .register_scene_symbol = nullptr,
  };
}

}  // namespace

TEST(PluginLoader, DiscoversSharedLibrariesByPlatformExtension) {
  const auto root = std::filesystem::temp_directory_path() / "manim_cpp_plugin_discovery";
  std::filesystem::remove_all(root);
  std::filesystem::create_directories(root);

  const auto plugin_path = root / ("sample_plugin" + std::string(kLibraryExtension));
  const auto text_path = root / "readme.txt";

  std::ofstream plugin_file(plugin_path);
  plugin_file << "binary";
  plugin_file.close();

  std::ofstream text_file(text_path);
  text_file << "not-a-plugin";
  text_file.close();

  const auto libraries = manim_cpp::plugin::PluginLoader::discover(root, false);
  ASSERT_EQ(libraries.size(), static_cast<size_t>(1));
  EXPECT_EQ(libraries[0].filename(), plugin_path.filename());
  std::filesystem::remove_all(root);
}

TEST(PluginLoader, RejectsUnsupportedHostAbiVersion) {
  std::string error;
  auto plugin = manim_cpp::plugin::PluginLoader::load(
      "/tmp/should-not-open", make_host_api(999), &error);

  EXPECT_EQ(plugin, nullptr);
  EXPECT_NE(error.find("Unsupported host ABI version"), std::string::npos);
}

TEST(PluginLoader, BatchLoadCollectsFailuresFromDiscoveredLibraries) {
  const auto root = std::filesystem::temp_directory_path() / "manim_cpp_plugin_batch";
  std::filesystem::remove_all(root);
  std::filesystem::create_directories(root);

#ifdef _WIN32
  const auto bad_library = root / "broken.dll";
#elif __APPLE__
  const auto bad_library = root / "broken.dylib";
#else
  const auto bad_library = root / "broken.so";
#endif
  std::ofstream out(bad_library);
  out << "not-a-real-library";
  out.close();

  const auto discovered = manim_cpp::plugin::PluginLoader::discover(root, false);
  ASSERT_EQ(discovered.size(), static_cast<size_t>(1));

  std::vector<std::string> errors;
  const auto loaded = manim_cpp::plugin::PluginLoader::load_discovered(
      discovered, make_host_api(MANIM_PLUGIN_ABI_VERSION_V1), &errors);

  EXPECT_TRUE(loaded.empty());
  EXPECT_EQ(errors.size(), static_cast<size_t>(1));

  std::filesystem::remove_all(root);
}

TEST(PluginLoader, LoadFromDirectoryComposesDiscoveryAndBatchLoad) {
  const auto root = std::filesystem::temp_directory_path() / "manim_cpp_plugin_load_from_dir";
  std::filesystem::remove_all(root);
  std::filesystem::create_directories(root);

#ifdef _WIN32
  const auto bad_library = root / "broken.dll";
#elif __APPLE__
  const auto bad_library = root / "broken.dylib";
#else
  const auto bad_library = root / "broken.so";
#endif
  std::ofstream out(bad_library);
  out << "still-not-a-real-library";
  out.close();

  std::vector<std::string> errors;
  const auto loaded = manim_cpp::plugin::PluginLoader::load_from_directory(
      root, false, make_host_api(MANIM_PLUGIN_ABI_VERSION_V1), &errors);

  EXPECT_TRUE(loaded.empty());
  EXPECT_EQ(errors.size(), static_cast<size_t>(1));

  std::filesystem::remove_all(root);
}

TEST(PluginLoader, LoadsValidSharedLibraryAndInvokesHostCallbacks) {
  ASSERT_TRUE(std::filesystem::exists(MANIM_CPP_TEST_PLUGIN_FIXTURE_PATH));

  HostCallbackState callback_state;
  callback_state.register_result = 0;
  g_host_callback_state = &callback_state;
  const manim_plugin_host_api_v1 host_api = {
      .abi_version = MANIM_PLUGIN_ABI_VERSION_V1,
      .log_message = &capture_log,
      .register_scene_symbol = &capture_registration,
  };

  std::string error;
  auto loaded = manim_cpp::plugin::PluginLoader::load(
      MANIM_CPP_TEST_PLUGIN_FIXTURE_PATH, host_api, &error);

  ASSERT_NE(loaded, nullptr) << error;
  EXPECT_TRUE(error.empty());
  EXPECT_EQ(callback_state.log_level, 1);
  EXPECT_EQ(callback_state.log_message, std::string("fixture plugin loaded"));
  EXPECT_EQ(callback_state.scene_name, std::string("FixtureScene"));
  EXPECT_EQ(callback_state.symbol_name, std::string("_fixture_scene_factory"));
  g_host_callback_state = nullptr;
}

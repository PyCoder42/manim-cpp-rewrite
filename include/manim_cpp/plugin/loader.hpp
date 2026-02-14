#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "manim_cpp/plugin/plugin_abi_v1.h"

namespace manim_cpp::plugin {

class LoadedPlugin {
 public:
  LoadedPlugin() = default;
  LoadedPlugin(void* handle, std::filesystem::path path);
  ~LoadedPlugin();

  LoadedPlugin(const LoadedPlugin&) = delete;
  LoadedPlugin& operator=(const LoadedPlugin&) = delete;

  LoadedPlugin(LoadedPlugin&& other) noexcept;
  LoadedPlugin& operator=(LoadedPlugin&& other) noexcept;

  bool valid() const { return handle_ != nullptr; }
  const std::filesystem::path& path() const { return path_; }

 private:
  void reset();

  void* handle_ = nullptr;
  std::filesystem::path path_;
};

class PluginLoader {
 public:
  static std::vector<std::filesystem::path> discover(
      const std::filesystem::path& root,
      bool recursive);

  static std::unique_ptr<LoadedPlugin> load(
      const std::filesystem::path& library_path,
      const manim_plugin_host_api_v1& host_api,
      std::string* error_message);
};

}  // namespace manim_cpp::plugin

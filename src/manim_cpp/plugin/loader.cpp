#include "manim_cpp/plugin/loader.hpp"

#include <utility>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace manim_cpp::plugin {

LoadedPlugin::LoadedPlugin(void* handle, std::filesystem::path path)
    : handle_(handle), path_(std::move(path)) {}

LoadedPlugin::~LoadedPlugin() { reset(); }

LoadedPlugin::LoadedPlugin(LoadedPlugin&& other) noexcept
    : handle_(other.handle_), path_(std::move(other.path_)) {
  other.handle_ = nullptr;
}

LoadedPlugin& LoadedPlugin::operator=(LoadedPlugin&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  reset();
  handle_ = other.handle_;
  path_ = std::move(other.path_);
  other.handle_ = nullptr;
  return *this;
}

void LoadedPlugin::reset() {
  if (handle_ == nullptr) {
    return;
  }
#ifdef _WIN32
  FreeLibrary(reinterpret_cast<HMODULE>(handle_));
#else
  dlclose(handle_);
#endif
  handle_ = nullptr;
}

std::unique_ptr<LoadedPlugin> PluginLoader::load(
    const std::filesystem::path& library_path,
    const manim_plugin_host_api_v1& host_api,
    std::string* error_message) {
#ifdef _WIN32
  HMODULE handle = LoadLibraryW(library_path.wstring().c_str());
  if (handle == nullptr) {
    if (error_message != nullptr) {
      *error_message = "LoadLibraryW failed";
    }
    return nullptr;
  }
  const auto init_fn = reinterpret_cast<manim_plugin_init_fn_v1>(
      GetProcAddress(handle, "manim_plugin_init"));
#else
  void* handle = dlopen(library_path.c_str(), RTLD_NOW | RTLD_LOCAL);
  if (handle == nullptr) {
    if (error_message != nullptr) {
      *error_message = dlerror();
    }
    return nullptr;
  }
  const auto init_fn = reinterpret_cast<manim_plugin_init_fn_v1>(
      dlsym(handle, "manim_plugin_init"));
#endif

  if (init_fn == nullptr) {
    if (error_message != nullptr) {
      *error_message = "Missing symbol: manim_plugin_init";
    }
#ifdef _WIN32
    FreeLibrary(handle);
#else
    dlclose(handle);
#endif
    return nullptr;
  }

  if (init_fn(&host_api) != 0) {
    if (error_message != nullptr) {
      *error_message = "Plugin initialization returned non-zero";
    }
#ifdef _WIN32
    FreeLibrary(handle);
#else
    dlclose(handle);
#endif
    return nullptr;
  }

  return std::make_unique<LoadedPlugin>(reinterpret_cast<void*>(handle),
                                        library_path);
}

}  // namespace manim_cpp::plugin

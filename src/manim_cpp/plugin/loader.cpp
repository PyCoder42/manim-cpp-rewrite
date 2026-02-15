#include "manim_cpp/plugin/loader.hpp"

#include <algorithm>
#include <cctype>
#include <string>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace manim_cpp::plugin {
namespace {

#ifdef _WIN32
std::string lowercase(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](const unsigned char c) {
                   return static_cast<char>(std::tolower(c));
                 });
  return value;
}
#endif

bool has_library_extension(const std::filesystem::path& path) {
#ifdef _WIN32
  return lowercase(path.extension().string()) == ".dll";
#elif __APPLE__
  return path.extension().string() == ".dylib";
#else
  return path.extension().string() == ".so";
#endif
}

}  // namespace

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

std::vector<std::filesystem::path> PluginLoader::discover(
    const std::filesystem::path& root,
    const bool recursive) {
  std::vector<std::filesystem::path> libraries;
  if (!std::filesystem::exists(root) || !std::filesystem::is_directory(root)) {
    return libraries;
  }

  if (recursive) {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
      if (!entry.is_regular_file() || !has_library_extension(entry.path())) {
        continue;
      }
      libraries.push_back(entry.path());
    }
  } else {
    for (const auto& entry : std::filesystem::directory_iterator(root)) {
      if (!entry.is_regular_file() || !has_library_extension(entry.path())) {
        continue;
      }
      libraries.push_back(entry.path());
    }
  }

  std::sort(libraries.begin(), libraries.end());
  return libraries;
}

std::vector<std::unique_ptr<LoadedPlugin>> PluginLoader::load_from_directory(
    const std::filesystem::path& root,
    const bool recursive,
    const manim_plugin_host_api_v1& host_api,
    std::vector<std::string>* errors) {
  return load_discovered(discover(root, recursive), host_api, errors);
}

std::vector<std::unique_ptr<LoadedPlugin>> PluginLoader::load_discovered(
    const std::vector<std::filesystem::path>& library_paths,
    const manim_plugin_host_api_v1& host_api,
    std::vector<std::string>* errors) {
  std::vector<std::unique_ptr<LoadedPlugin>> loaded;
  loaded.reserve(library_paths.size());

  for (const auto& path : library_paths) {
    std::string error;
    auto plugin = load(path, host_api, &error);
    if (plugin == nullptr) {
      if (errors != nullptr) {
        errors->push_back(path.string() + ": " + error);
      }
      continue;
    }
    loaded.push_back(std::move(plugin));
  }

  return loaded;
}

std::unique_ptr<LoadedPlugin> PluginLoader::load(
    const std::filesystem::path& library_path,
    const manim_plugin_host_api_v1& host_api,
    std::string* error_message) {
  if (host_api.abi_version != MANIM_PLUGIN_ABI_VERSION_V1) {
    if (error_message != nullptr) {
      *error_message =
          "Unsupported host ABI version: " + std::to_string(host_api.abi_version);
    }
    return nullptr;
  }

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

#pragma once

#include <cstdint>
#include <utility>

#include "manim_cpp/plugin/plugin_abi_v1.h"

namespace manim_cpp::plugin::sdk {

inline constexpr uint32_t kSupportedAbi = MANIM_PLUGIN_ABI_VERSION_V1;
inline constexpr int kInvalidHostAbiStatus = 1;

inline bool host_api_is_compatible(const manim_plugin_host_api_v1* host_api) {
  return host_api != nullptr && host_api->abi_version == kSupportedAbi;
}

class HostApiView {
 public:
  explicit HostApiView(const manim_plugin_host_api_v1* host_api)
      : host_api_(host_api) {}

  [[nodiscard]] bool valid() const { return host_api_is_compatible(host_api_); }

  void log(const int level, const char* message) const {
    if (!valid() || host_api_->log_message == nullptr) {
      return;
    }
    host_api_->log_message(level, message);
  }

  [[nodiscard]] bool register_scene(const char* scene_name,
                                    const char* symbol_name) const {
    if (!valid() || host_api_->register_scene_symbol == nullptr) {
      return false;
    }
    return host_api_->register_scene_symbol(scene_name, symbol_name) == 0;
  }

 private:
  const manim_plugin_host_api_v1* host_api_ = nullptr;
};

template <typename InitFn>
int initialize_plugin(const manim_plugin_host_api_v1* host_api,
                      InitFn&& init_fn) {
  const HostApiView host(host_api);
  if (!host.valid()) {
    return kInvalidHostAbiStatus;
  }
  return static_cast<int>(std::forward<InitFn>(init_fn)(host));
}

}  // namespace manim_cpp::plugin::sdk

#pragma once

#include "manim_cpp/plugin/plugin_abi_v1.h"

namespace manim_cpp::plugin::sdk {

inline constexpr uint32_t kSupportedAbi = MANIM_PLUGIN_ABI_VERSION_V1;

inline bool host_api_is_compatible(const manim_plugin_host_api_v1* host_api) {
  return host_api != nullptr && host_api->abi_version == kSupportedAbi;
}

}  // namespace manim_cpp::plugin::sdk

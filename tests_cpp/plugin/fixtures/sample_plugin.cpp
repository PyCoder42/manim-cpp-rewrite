#include "manim_cpp/plugin/plugin_abi_v1.h"

extern "C" int manim_plugin_init(const manim_plugin_host_api_v1* host_api) {
  if (host_api == nullptr || host_api->abi_version != MANIM_PLUGIN_ABI_VERSION_V1) {
    return 1;
  }

  if (host_api->log_message != nullptr) {
    host_api->log_message(1, "fixture plugin loaded");
  }
  if (host_api->register_scene_symbol != nullptr) {
    return host_api->register_scene_symbol("FixtureScene", "_fixture_scene_factory");
  }
  return 0;
}

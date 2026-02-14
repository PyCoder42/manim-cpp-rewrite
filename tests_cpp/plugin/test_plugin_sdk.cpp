#include <string>

#include <gtest/gtest.h>

#include "../../plugins/sdk/plugin_sdk.hpp"

namespace {

struct CallbackState {
  int log_level = -1;
  std::string log_message;
  std::string scene_name;
  std::string symbol_name;
  int register_result = 0;
};

CallbackState* g_state = nullptr;

void record_log(int level, const char* message) {
  if (g_state == nullptr) {
    return;
  }
  g_state->log_level = level;
  g_state->log_message = message != nullptr ? message : "";
}

int record_registration(const char* scene_name, const char* symbol_name) {
  if (g_state == nullptr) {
    return 1;
  }
  g_state->scene_name = scene_name != nullptr ? scene_name : "";
  g_state->symbol_name = symbol_name != nullptr ? symbol_name : "";
  return g_state->register_result;
}

}  // namespace

TEST(PluginSdk, HostApiViewValidityTracksAbiCompatibility) {
  const manim_plugin_host_api_v1 valid_host_api = {
      .abi_version = MANIM_PLUGIN_ABI_VERSION_V1,
      .log_message = nullptr,
      .register_scene_symbol = nullptr,
  };
  const manim_plugin_host_api_v1 invalid_host_api = {
      .abi_version = 0,
      .log_message = nullptr,
      .register_scene_symbol = nullptr,
  };

  manim_cpp::plugin::sdk::HostApiView valid_view(&valid_host_api);
  manim_cpp::plugin::sdk::HostApiView invalid_view(&invalid_host_api);

  EXPECT_TRUE(valid_view.valid());
  EXPECT_FALSE(invalid_view.valid());
}

TEST(PluginSdk, HostApiViewDispatchesCallbacksWhenPresent) {
  CallbackState state;
  g_state = &state;
  const manim_plugin_host_api_v1 host_api = {
      .abi_version = MANIM_PLUGIN_ABI_VERSION_V1,
      .log_message = &record_log,
      .register_scene_symbol = &record_registration,
  };

  manim_cpp::plugin::sdk::HostApiView view(&host_api);
  ASSERT_TRUE(view.valid());

  view.log(2, "plugin loaded");
  EXPECT_EQ(state.log_level, 2);
  EXPECT_EQ(state.log_message, std::string("plugin loaded"));

  state.register_result = 0;
  EXPECT_TRUE(view.register_scene("DemoScene", "_demo_scene_factory"));
  EXPECT_EQ(state.scene_name, std::string("DemoScene"));
  EXPECT_EQ(state.symbol_name, std::string("_demo_scene_factory"));

  state.register_result = 7;
  EXPECT_FALSE(view.register_scene("DemoScene", "_demo_scene_factory"));
  g_state = nullptr;
}

TEST(PluginSdk, InitializePluginRejectsIncompatibleAbiWithoutHandlerCall) {
  const manim_plugin_host_api_v1 host_api = {
      .abi_version = 0,
      .log_message = nullptr,
      .register_scene_symbol = nullptr,
  };
  int invocation_count = 0;

  const int result = manim_cpp::plugin::sdk::initialize_plugin(
      &host_api, [&](const manim_cpp::plugin::sdk::HostApiView&) {
        ++invocation_count;
        return 0;
      });

  EXPECT_EQ(result, 1);
  EXPECT_EQ(invocation_count, 0);
}

TEST(PluginSdk, InitializePluginInvokesHandlerForCompatibleAbi) {
  CallbackState state;
  g_state = &state;
  const manim_plugin_host_api_v1 host_api = {
      .abi_version = MANIM_PLUGIN_ABI_VERSION_V1,
      .log_message = &record_log,
      .register_scene_symbol = &record_registration,
  };
  int invocation_count = 0;
  state.register_result = 0;

  const int result = manim_cpp::plugin::sdk::initialize_plugin(
      &host_api, [&](const manim_cpp::plugin::sdk::HostApiView& host) {
        ++invocation_count;
        host.log(3, "init");
        return host.register_scene("InitScene", "_init_scene_factory") ? 0 : 5;
      });

  EXPECT_EQ(result, 0);
  EXPECT_EQ(invocation_count, 1);
  EXPECT_EQ(state.log_level, 3);
  EXPECT_EQ(state.log_message, std::string("init"));
  EXPECT_EQ(state.scene_name, std::string("InitScene"));
  EXPECT_EQ(state.symbol_name, std::string("_init_scene_factory"));
  g_state = nullptr;
}

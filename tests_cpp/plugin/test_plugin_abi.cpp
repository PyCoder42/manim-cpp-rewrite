#include <cstdint>
#include <type_traits>

#include <gtest/gtest.h>

#include "manim_cpp/plugin/plugin_abi_v1.h"

TEST(PluginAbi, DeclaresExpectedVersionAndStructLayout) {
  EXPECT_EQ(MANIM_PLUGIN_ABI_VERSION_V1, static_cast<uint32_t>(1));
  EXPECT_TRUE((std::is_standard_layout_v<manim_plugin_host_api_v1>));
}

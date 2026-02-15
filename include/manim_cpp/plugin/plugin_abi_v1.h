#ifndef MANIM_CPP_PLUGIN_ABI_V1_H_
#define MANIM_CPP_PLUGIN_ABI_V1_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MANIM_PLUGIN_ABI_VERSION_V1 ((uint32_t)1)

typedef struct manim_plugin_host_api_v1 {
  uint32_t abi_version;
  void (*log_message)(int level, const char* message);
  int (*register_scene_symbol)(const char* scene_name,
                               const char* symbol_name);
} manim_plugin_host_api_v1;

typedef int (*manim_plugin_init_fn_v1)(
    const manim_plugin_host_api_v1* host_api);

#ifdef __cplusplus
}
#endif

#endif  // MANIM_CPP_PLUGIN_ABI_V1_H_

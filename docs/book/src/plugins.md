# Plugins

Plugins are loaded through a stable C ABI boundary with version gating.

## ABI Contract

Required symbol:

```c
extern "C" int manim_plugin_init(const manim_plugin_host_api_v1* host);
```

Host version check is enforced with `MANIM_PLUGIN_ABI_VERSION_V1`.

## CLI Workflow

```sh
manim-cpp plugins path
manim-cpp plugins list
manim-cpp plugins load
```

Set `MANIM_CPP_PLUGIN_DIR` to override the default plugin directory at runtime.

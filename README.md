# manim-cpp

`manim-cpp` is a C++23 animation engine monorepo focused on Manim CE behavior parity with dual Cairo/OpenGL rendering, deterministic media outputs, plugin ABI support, and a Python-free mainline.

## Current State

- Primary CLI: `manim-cpp`
- Transitional aliases (one major version): `manim`, `manimce`
- Migration tool: `manim-cpp-migrate`
- Frame extractor: `manim-cpp-extract-frames`
- Build baseline: C++23 (`GCC 13+`, `Clang 17+`, `MSVC 19.38+`)

## Build Quick Start

Install system dependencies first:

- Linux: `./tools/deps/install_linux.sh`
- macOS: `./tools/deps/install_macos.sh`
- Windows: `./tools/deps/install_windows.ps1`

Then configure/build/test:

```sh
cmake --preset debug
cmake --build --preset debug -j4
ctest --preset debug --output-on-failure
```

Release build:

```sh
cmake --preset release
cmake --build --preset release -j4
ctest --preset release --output-on-failure
```

## CLI Usage

Show help/version:

```sh
manim-cpp --help
manim-cpp --version
```

Initialize a project scaffold:

```sh
manim-cpp init project ./demo_project
```

Render a registered C++ scene:

```sh
manim-cpp render demo_scene.cpp --scene MyScene --renderer cairo --format mp4
```

## Repository Layout

- Runtime: `src/manim_cpp/`
- Public headers: `include/manim_cpp/`
- Primary CLI app: `apps/manim_cpp_cli/`
- Migration app: `apps/manim_cpp_migrate/`
- Plugin SDK/ABI: `plugins/sdk/`
- Tests: `tests_cpp/`
- Docs: `docs/book/` (mdBook), `docs/api/` (Doxygen)
- Tooling: `tools/`

## Rendering and Media

- Renderers: Cairo, OpenGL
- Formats: `png`, `gif`, `mp4`, `webm`, `mov`
- Deterministic frame/media payloads for bitwise regression checks
- Subcaptions (`.srt`), section timeline metadata, codec hint metadata

## Plugins

Plugin loading is backed by a stable C ABI boundary:

- Entrypoint: `extern "C" int manim_plugin_init(const manim_plugin_host_api_v1*)`
- Host ABI gate: `MANIM_PLUGIN_ABI_VERSION_V1`

List and load plugins:

```sh
manim-cpp plugins path
manim-cpp plugins list
manim-cpp plugins load
```

## Migration Tool

Convert supported scene constructs into deterministic C++ output:

```sh
manim-cpp-migrate input_scene.py --out-dir converted/
manim-cpp-migrate scenes_dir --recursive --out-dir converted/
```

## Documentation

- mdBook sources: `docs/book/src/`
- Doxygen config: `docs/api/Doxyfile`
- Build docs:

```sh
./tools/docs/build_docs.sh
```

## License

MIT. See `LICENSE` and `LICENSE.community`.

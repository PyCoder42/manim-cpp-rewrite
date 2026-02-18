# Architecture

The runtime is organized under `manim_cpp` namespaces and linked into shared CLI
binaries.

## Core Namespaces

- `manim_cpp::scene`: scene lifecycle, timeline, and file writer contracts.
- `manim_cpp::mobject`: geometry and scene-graph data model.
- `manim_cpp::animation`: animation primitives and composition APIs.
- `manim_cpp::renderer`: Cairo/OpenGL renderer contracts and interaction state.
- `manim_cpp::camera`: camera frame/rate/pixel configuration.
- `manim_cpp::config`: `manim.cfg` loading, precedence, and path templates.
- `manim_cpp::plugin`: ABI-compatible plugin discovery and loading.

## Executables

- `apps/manim_cpp_cli`: main `manim-cpp` CLI entrypoint.
- `apps/manim_alias_manim`, `apps/manim_alias_manimce`: transitional aliases.
- `apps/manim_cpp_migrate`: migration utility.
- `apps/manim_cpp_extract_frames`: `.npz` frame extraction utility.

# Architecture

The runtime is organized under `manim_cpp` namespaces:

- `scene`
- `mobject`
- `animation`
- `renderer`
- `camera`
- `config`
- `plugin`

CLI and migration binaries live in `apps/` and link `manim_cpp_core`.

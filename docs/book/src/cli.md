# CLI Reference

Primary binary: `manim-cpp`.

## Root Commands

- `render`: render scenes from a C++ input unit.
- `cfg`: inspect/write config templates.
- `checkhealth`: inspect runtime dependencies and supported formats/renderers.
- `init`: scaffold scene/project structure.
- `plugins`: path/list/load plugin libraries.

## Useful Render Flags

- `--scene <SceneName>`
- `--renderer <cairo|opengl>`
- `--format <png|gif|mp4|webm|mov>`
- `--watch`, `--interactive`
- `--interaction_script <path>`
- `--enable_gui`, `--fullscreen`, `--force_window`
- `--window_position`, `--window_size`, `--window_monitor`

Use `manim-cpp render --help` for the full flag contract.

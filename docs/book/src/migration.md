# Migration

`manim-cpp-migrate` provides deterministic source-to-source conversion for
supported scene patterns.

## Supported Patterns

- Scene class detection for common scene base classes.
- Translation of selected calls such as `wait`, `clear`, and seed/updater helpers.
- Constructor-safe translation for supported `add/remove` geometry expressions.

## Unsupported Dynamic Patterns

Unsupported constructs are preserved as explicit `TODO(migrate)` annotations in
generated C++ output.

## Usage

```sh
manim-cpp-migrate input_scene.py --out-dir converted/
manim-cpp-migrate scenes_dir --recursive --out-dir converted/
```

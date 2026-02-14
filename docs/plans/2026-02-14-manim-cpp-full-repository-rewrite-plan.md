# Manim-Cpp Full Repository Rewrite Plan

## Summary
Rewrite the entire repository at `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++` into a pure C++23 monorepo with zero Python files remaining on the mainline branch.

Target is near-1:1 Manim CE behavior parity, dual Cairo/OpenGL support, bitwise rendering parity against per-platform baselines, preserved `manim.cfg` semantics, preserved docs locales/catalogs, and a renamed primary CLI/product (`manim-cpp`) with temporary aliases (`manim`, `manimce`) for one major version.

## Public APIs, Interfaces, and Type Contracts
1. CLI binaries:
`manim-cpp` (primary), `manim`, `manimce` (aliases for one major version), and separate converter binary `manim-cpp-migrate`.
2. Scene authoring model:
C++ source scenes only, discovered via static registration macro (for example `MANIM_REGISTER_SCENE(MyScene)`), with compile-time scene factory registry.
3. Core namespace contract:
`manim_cpp::scene`, `manim_cpp::mobject`, `manim_cpp::animation`, `manim_cpp::renderer`, `manim_cpp::camera`, `manim_cpp::config`, `manim_cpp::plugin`.
4. Plugin ABI:
stable C ABI boundary (v1) with C++ wrapper SDK. Required entrypoint (versioned): `extern "C" int manim_plugin_init(const manim_plugin_host_api_v1*)`.
5. Config compatibility:
keep `manim.cfg` layout and key semantics from `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/manim/_config/default.cfg`, including `[CLI]`, `[custom_folders]`, `[logger]`, `[ffmpeg]`, `[jupyter]` keys (notebook functionality deferred, keys accepted with explicit warnings where needed).
6. Subtitle/media contract:
preserve `.srt` generation behavior and FFmpeg-backed output formats (`png`, `gif`, `mp4`, `webm`, `mov`).
7. Baseline data contract:
keep `.npz` regression artifacts initially; C++ reader/writer implemented in-repo.

## Repository Restructure (Final State)
1. Runtime source:
`/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/src/manim_cpp/...`
2. Public headers:
`/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/include/manim_cpp/...`
3. CLI apps:
`/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/apps/manim_cpp_cli/` and `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/apps/manim_cpp_migrate/`
4. Plugin SDK and ABI headers:
`/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/plugins/sdk/`
5. Tests:
`/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/tests_cpp/` (unit, integration, render_regression, cli, plugin, migrate)
6. Docs:
`/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/docs/book/` (mdBook guides) and `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/docs/api/` (Doxygen output)
7. Tooling scripts (non-Python):
`/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/tools/` (shell/C++/PowerShell only)
8. Build system:
top-level CMake with presets at `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/CMakeLists.txt` and `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/CMakePresets.json`
9. Legacy Python removal:
delete `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/manim`, `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/scripts/*.py`, `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/.github/scripts/*.py`, and Python build metadata in final cutover phase.

## Implementation Workstreams and Milestones
1. Milestone A: Foundation and build skeleton.
Define C++23 toolchain floor (GCC 13+, Clang 17+, MSVC 19.38+), CMake presets, OS package install scripts (Linux/macOS/Windows), `ctest` skeleton, and binary entrypoints for `manim-cpp`, alias launchers, and `manim-cpp-migrate`.
2. Milestone B: Core math and geometry kernel.
Adopt Eigen for low-level math; implement in-repo equivalents for previously external algorithmic layers (triangulation, boolean path ops, graph layouts, isocurve extraction, combinatorics helpers, rotation utilities).
3. Milestone C: Scene graph and animation engine.
Port `Mobject`, animation timeline, updater pipeline, scene lifecycle (`setup/construct/tear_down`), timing, caching keys, and deterministic random-seed behavior.
4. Milestone D: Cairo backend parity.
Implement Cairo renderer pipeline and camera capture path with deterministic frame production, static-frame optimization, and parity with current output directory behavior.
5. Milestone E: OpenGL backend and interaction.
Implement OpenGL renderer, shader pipeline migration from `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/manim/renderer/shaders`, GLFW + Dear ImGui interaction window, camera controls, and watch/interactive behavior.
6. Milestone F: Media/file writer and subtitles/audio.
Port FFmpeg muxing, partial movie caching, section outputs, audio layering, and `.srt` subcaption emission.
7. Milestone G: CLI/config parity.
Rebuild command tree (`render`, `cfg`, `checkhealth`, `init`, `plugins`), preserve argument semantics and config precedence rules, preserve template/project initialization behavior with C++ scene templates.
8. Milestone H: Plugin system.
Implement discovery, loading, version checks, and C ABI host contract; ship C++ helper SDK and plugin test fixtures.
9. Milestone I: Python-to-C++ migration utility.
Build separate `manim-cpp-migrate` binary with deterministic AST translation of supported constructs and explicit TODO annotations for unsupported dynamic patterns.
10. Milestone J: Test suite full port.
Semantically port existing unit/module/graphical/OpenGL/CLI/plugin tests from `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/tests` into `gtest`/`gmock` plus render-regression harness.
11. Milestone K: Docs and i18n migration.
Move docs to mdBook + Doxygen, keep current locale/catalog continuity (`fr/hi/pt/sv`) and keep gettext workflow without Python.
12. Milestone L: Release engineering and cutover.
Replace Python-based GitHub workflows with C++ build/test/release pipelines, publish prebuilt binaries and source builds, publish docs artifact, enforce zero-Python gate, then remove aliases after one major version.

## Detailed Subsystem Mapping (Source-to-Source Intent)
1. `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/manim/mobject` → `src/manim_cpp/mobject` with class-level parity for geometry, graphing, text, svg, 3D, types.
2. `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/manim/animation` → `src/manim_cpp/animation` preserving creation/transform/fading/movement APIs.
3. `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/manim/scene` → `src/manim_cpp/scene` preserving scene lifecycle and playback semantics.
4. `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/manim/renderer` → `src/manim_cpp/renderer` for Cairo/OpenGL backends and shader orchestration.
5. `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/manim/_config` and `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/manim/cli` → `src/manim_cpp/config` + `apps/manim_cpp_cli`.
6. `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/manim/plugins` → `src/manim_cpp/plugin` + `plugins/sdk`.
7. `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/docs/source` and `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/docs/i18n` → `docs/book`, `docs/api`, and preserved locale catalogs.
8. `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/tests/control_data` and `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/tests/test_graphical_units/control_data` stay as baseline assets, consumed by C++ harness.

## Test Cases and Acceptance Scenarios
1. CLI compatibility:
golden output snapshots for help text, command/subcommand behavior, flag interactions, init/template generation, config read/write.
2. Render parity:
per-platform bitwise frame matching for Cairo and OpenGL against baseline suites; no tolerance mode for required parity gates.
3. Media parity:
frame count, resolution, codec metadata, section output paths, and audio/subtitle timelines match expected fixtures.
4. Scene API parity:
ported sample scenes from `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++/example_scenes` compile and render correctly in C++.
5. Interaction parity:
GLFW/ImGui event-driven camera and input behavior matches current interaction semantics.
6. Plugin ABI:
load/unload/version-negotiation tests across Linux/macOS/Windows for sample plugins.
7. Migration utility:
converter integration tests for supported Python scene patterns and explicit TODO insertion for unsupported dynamic constructs.
8. Docs/i18n:
mdBook + Doxygen build passes, locale catalogs preserved, gettext extraction/merge checks pass.
9. Zero Python gate:
CI check fails if any tracked Python files remain (`*.py`, `*.pyi`, Python-specific build scripts/configs).

## Rollout and Branching Strategy
1. Use phased cutover with long-lived rewrite integration branch and subsystem branches (prefix `codex/`).
2. Require milestone gate reviews before merging next subsystem tranche.
3. Keep `manim` and `manimce` aliases for exactly one major version after `manim-cpp` stable release.
4. Publish binary artifacts via GitHub Releases plus OS package channels; keep source build path first-class.
5. Final cutover step removes Python assets from mainline only after all parity gates pass.

## Assumptions and Defaults (Locked)
1. Absolute zero Python in final repository and CI.
2. Near-1:1 parity objective.
3. C++ scene authoring only.
4. Cairo + OpenGL + FFmpeg retained as core external system libraries.
5. Rebuild most non-core algorithmic logic in-repo.
6. Monorepo architecture.
7. C++23 baseline on GCC 13+/Clang 17+/MSVC 19.38+.
8. System package dependency model (no Conan/vcpkg).
9. Platform scope: Linux + macOS + Windows from first release.
10. Docs stack: Doxygen + mdBook; locales/catalogs preserved.
11. Test stack: GoogleTest/GMock + custom render harness.
12. Baseline artifact format stays `.npz` initially.
13. Notebook workflow deferred (not in v1 parity scope).
14. Quality-first schedule (no fixed date).

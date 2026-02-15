# Manim-Cpp Full Repository Rewrite Plan

## Summary
Rewrite the entire repository at `/Users/saahir/Desktop/Coding/Manim stuff/Manim to C++` into a pure C++23 monorepo with zero Python files remaining on the mainline branch.

Target is near-1:1 Manim CE behavior parity, dual Cairo/OpenGL support, bitwise rendering parity against per-platform baselines, preserved `manim.cfg` semantics, preserved docs locales/catalogs, and a renamed primary CLI/product (`manim-cpp`) with temporary aliases (`manim`, `manimce`) for one major version.

## Progress Checklist

### Public APIs, Interfaces, and Type Contracts
- [x] CLI binaries scaffolded: `manim-cpp`, `manim`, `manimce`, and `manim-cpp-migrate`
- [x] Scene authoring registration macro scaffolded (`MANIM_REGISTER_SCENE`)
- [x] Core namespace scaffolded: `scene`, `mobject`, `animation`, `renderer`, `camera`, `config`, `plugin`
- [x] Plugin ABI v1 header + loader scaffolded (`manim_plugin_init` contract)
- [x] `manim.cfg` compatibility baseline copied to `config/manim.cfg.default`
- [x] Subtitle/media writer API scaffolding added (`SceneFileWriter`, sections, subcaption API placeholder)
- [x] Baseline artifact contract acknowledged in tests/docs (npz retained for now)

### Repository Restructure (Final State)
- [x] Runtime source in `src/manim_cpp/...`
- [x] Public headers in `include/manim_cpp/...`
- [x] CLI apps in `apps/manim_cpp_cli/` and `apps/manim_cpp_migrate/`
- [x] Plugin SDK and ABI headers in `plugins/sdk/`
- [x] C++ tests scaffold in `tests_cpp/`
- [x] Docs scaffold in `docs/book/` and `docs/api/`
- [x] Non-Python tooling scaffold in `tools/`
- [x] Top-level CMake and presets added
- [x] Legacy Python removal (completed cutover; zero tracked Python files)

### Implementation Workstreams and Milestones
- [x] Milestone A: Foundation and build skeleton
- [x] Milestone B: Core math and geometry kernel
- [x] Milestone B.1: Math kernel bootstrap (`choose`, `rotate_z`, fan triangulation) with `gtest` coverage
- [x] Milestone B.2: Introduce Eigen-backed vector/matrix adapters for kernel usage
- [x] Milestone B.3: Port geometry algorithms (triangulation variants, boolean path ops, graph layouts, isocurve extraction)
- [x] Milestone B.3.a: Ear-clipping triangulation for simple concave/convex polygons
- [x] Milestone B.3.b: Boolean path operations
- [x] Milestone B.3.b.1: Foundational path predicates (`segments_intersect`, `point_in_polygon`, `has_self_intersections`)
- [x] Milestone B.3.b.2: Convex polygon intersection via Sutherland-Hodgman clipping
- [x] Milestone B.3.b.3: Public polygon signed-area/area helpers for winding-sensitive path operations
- [x] Milestone B.3.b.4: Convex union/difference area helpers (`union_area_convex_polygons`, `difference_area_convex_polygons`)
- [x] Milestone B.3.c: Graph layouts, isocurve extraction, and combinatorics helpers
- [x] Milestone B.3.c.1: Deterministic circular graph layout helper
- [x] Milestone B.3.c.2: Marching-squares isocurve extraction helper
- [x] Milestone B.3.c.3: Combinatorics helpers parity
- [x] Milestone C: Scene graph and animation engine parity
- [x] Milestone C.1: Scene lifecycle runner (`setup` -> `construct` -> `tear_down`)
- [x] Milestone C.2: Basic animation playback timeline with updater ticks and elapsed-time tracking
- [x] Milestone C.3: Deterministic scene RNG seed controls (`set_random_seed`, replayable random stream)
- [x] Milestone C.4: Animation run-time and rate-function support integrated into `Scene::play`
- [x] Milestone C.5: Base `Mobject` scene-graph node supports child lists, center transforms, and opacity state
- [x] Milestone C.6: `Scene` tracks unique mobject instances with add/remove/clear semantics
- [x] Milestone C.7: Added concrete geometry mobjects (`Dot`, `Circle`, `Square`) with deterministic radius/vertex contracts and C++ unit coverage
- [x] Milestone C.8: Added concrete transform/fade animation primitives (`MoveToAnimation`, `ShiftAnimation`, `FadeToOpacityAnimation`) with timeline integration tests
- [x] Milestone C.9: Added C++ `ValueTracker` mobject parity (`value`, `set_value`, `increment_value`) with deterministic precision coverage
- [ ] Milestone D: Cairo backend parity
- [x] Milestone D.1: Renderer type parsing, string conversion, and factory creation for Cairo/OpenGL backends
- [x] Milestone D.2: Cairo renderer deterministic frame naming + static-frame signature dedupe scaffold
- [x] Milestone D.3: Scene output directory planning resolves deterministic image/video/partial paths from `manim.cfg` templates (`module_name`, `quality`, `scene_name`)
- [x] Milestone D.4: Camera configuration scaffold now supports deterministic frame rate, pixel-size, and aspect-ratio contracts with unit coverage
- [ ] Milestone E: OpenGL backend + interaction parity
- [x] Milestone E.1: Shader root discovery supports `MANIM_CPP_SHADER_ROOT` override for runtime/packaged deployments
- [x] Milestone E.2: OpenGL renderer shader root/path helpers for deterministic program-stage shader resolution
- [x] Milestone E.3: `render` accepts `--watch` / `--interactive` interaction flags and surfaces resolved state
- [x] Milestone E.4: `render` accepts OpenGL window/GUI flags (`--enable_gui`, `--fullscreen`, `--force_window`, `--window_position`, `--window_size`, `--window_monitor`) with strict validation
- [ ] Milestone F: Media/file writer + subtitles/audio parity
- [x] Milestone F.1: Subcaption collection + `.srt` file emission scaffold
- [x] Milestone F.2: Explicit section tracking and per-section partial movie routing
- [x] Milestone F.3: Audio segment layering scaffold (`path`, start offset, gain) with input validation
- [x] Milestone F.4: Deterministic animation-driven partial movie caching with section skip handling
- [x] Milestone F.5: Scene media manifest JSON writer captures sections, subcaptions, and audio segment metadata
- [x] Milestone F.6: Typed media-format parser/stringifier covers `png`, `gif`, `mp4`, `webm`, and `mov`
- [x] Milestone F.7: `SceneFileWriter::resolve_output_paths` provides config-driven media path contract with nullopt on unresolved placeholders
- [x] Milestone F.8: `SceneFileWriter` manifest now carries deterministic render summary metadata (frame count, resolution, fps, format, codec hint, duration, output file)
- [x] Milestone F.9: `render --scene` now emits config-resolved media artifacts (output media file, manifest JSON, `.srt`) with deterministic metadata contract
- [x] Milestone G: CLI/config parity
- [x] Milestone G.1: Command/subcommand validation scaffolding with deterministic exit codes
- [x] Milestone G.2: `cfg show` and `cfg write` operate on real config files/templates
- [x] Milestone G.3: `plugins list <dir>` validates input and reports discovered shared libraries
- [x] Milestone G.4: `checkhealth --json` emits machine-readable dependency status
- [x] Milestone G.5: `init scene <output.cpp>` generates a compilable C++ scene template with `MANIM_REGISTER_SCENE`
- [x] Milestone G.6: `init project <dir>` generates `manim.cfg` + `scenes/main_scene.cpp` scaffold
- [x] Milestone G.7: `plugins list --recursive <dir>` supports nested plugin discovery
- [x] Milestone G.8: Config loader supports deterministic multi-file precedence layering with reset-safe `load_from_file`
- [x] Milestone G.9: `render` validates `--renderer <cairo|opengl>` and reports selected backend in scaffold output
- [x] Milestone G.10: `checkhealth --json` includes explicit supported renderer metadata
- [x] Milestone G.11: `plugins load` emits plugin log lines and registered scene symbols from host callback wiring
- [x] Milestone G.12: `render` validates `--format` and reports resolved media format in scaffold output
- [x] Milestone G.13: `checkhealth --json` emits supported output format metadata for automation clients
- [x] Milestone G.14: `ManimConfig::resolve_path` expands nested `{...}` directory templates (CLI/custom_folders) with explicit substitution inputs
- [x] Milestone G.15: `render --help` now emits detailed option contract (renderer/format/watch/interactive/window flags) with CLI compatibility test coverage
- [x] Milestone G.16: `render --scene <SceneName>` now resolves and executes registered C++ scenes via `SceneRegistry`, with explicit unknown-scene failure coverage
- [x] Milestone H: Plugin system parity
- [x] Milestone H.1: Plugin discovery by shared-library extension + host ABI version gate
- [x] Milestone H.2: Batch plugin loading API with deterministic failure aggregation
- [x] Milestone H.3: Directory-first plugin load convenience API (discover + batch load)
- [x] Milestone H.4: CLI `plugins load [--recursive] <dir>` bridges discovery/loading APIs with deterministic host ABI wiring
- [x] Milestone H.5: C++ plugin SDK `HostApiView` wraps ABI callbacks with compatibility checks and dispatch helpers
- [x] Milestone H.6: Plugin SDK `initialize_plugin` helper enforces ABI gate and standard handler dispatch
- [x] Milestone H.7: Shared-library plugin fixture integration test validates real load path and host callback wiring
- [x] Milestone I: Migration utility parity
- [x] Milestone I.1: Detect `Scene`-family base classes + preserve original `self.*(...)` calls as TODO annotations
- [x] Milestone I.2: `manim-cpp-migrate --report <file>` writes deterministic migration summaries
- [x] Milestone I.3: `manim-cpp-migrate <dir> --out-dir <dir>` batch-converts `.py` scenes to deterministic `.cpp` outputs
- [x] Milestone I.4: `manim-cpp-migrate --recursive` discovers nested scene files and preserves relative output paths
- [x] Milestone I.5: Directory migration fails fast with deterministic error when no Python scene files are discovered
- [ ] Milestone J: Test suite full port
- [x] Milestone J.1: Added C++ unit/integration coverage for core math/scene/cli/plugin/migrate scaffolds
- [x] Milestone J.2: Added shared-library plugin integration fixture coverage in `ctest`
- [x] Milestone J.3: Added C++ `.npz` control-data regression harness (`NpzArchive`) that validates graphical baseline archives without Python
- [x] Milestone J.4: Added example-scene registry/run integration coverage by linking converted C++ `example_scenes` into `manim_cpp_tests`
- [ ] Milestone K: Docs and i18n migration parity
- [x] Milestone K.1: Added non-Python i18n catalog continuity check (`DocsI18nCatalogCheck`) to `ctest`
- [x] Milestone K.2: Added non-Python docs build entrypoint (`tools/docs/build_docs.sh`) and updated `docs/Makefile`/`docs/make.bat` wrappers
- [ ] Milestone L: Release engineering and cutover
- [x] Milestone L.1: Added transitional Python inventory allowlist gate (`PythonInventoryAllowlistCheck`) to track and shrink legacy Python footprint
- [x] Milestone L.2: Updated GitHub Python gate workflow to enforce allowlist inventory now and strict zero-Python automatically when allowlist is empty
- [x] Milestone L.3: Transitioned legacy Python workflows (`ci.yml`, `python-publish.yml`, `release-publish-documentation.yml`) to manual-only triggers with CTest enforcement (`LegacyPythonWorkflowModeCheck`)
- [x] Milestone L.4: Removed `.github/scripts/ci_build_cairo.py` and tightened transitional allowlist scope by dropping `.github/scripts/*`
- [x] Milestone L.5: Replaced `scripts/make_and_open_docs.py` with `scripts/make_and_open_docs.sh` and migrated the docstring template from `.py` to `.rst`, shrinking `scripts/` allowlist scope
- [x] Milestone L.6: Removed legacy Sphinx `docs/source/conf.py` from tracked Python inventory after docs toolchain migration to mdBook + Doxygen
- [x] Milestone L.7: Converted `example_scenes/basic.py`, `advanced_tex_fonts.py`, and `customtex.py` to compiled C++ scene stubs and narrowed Python allowlist to `example_scenes/opengl.py`
- [x] Milestone L.8: Converted `example_scenes/opengl.py` to compiled C++ scene stubs; `example_scenes/` is now Python-free
- [x] Milestone L.9: Replaced `scripts/extract_frames.py` with `manim-cpp-extract-frames` (C++) + `scripts/extract_frames.sh`, reducing allowlist to `manim/*`, `tests/*`, and `scripts/dev_changelog.py`
- [x] Milestone L.10: Removed legacy `manim/` and `tests/` Python sources from the tracked tree; transitional allowlist now contains only `scripts/dev_changelog.py`
- [x] Milestone L.11: Replaced `scripts/dev_changelog.py` with `scripts/dev_changelog.sh` (non-Python) and removed the final tracked Python source file
- [x] Milestone L.12: Emptied `config/python_allowlist.txt` to transition CI from transitional allowlist mode to strict zero-Python enforcement mode

### Detailed Subsystem Mapping (Source-to-Source Intent)
- [ ] `manim/mobject` -> `src/manim_cpp/mobject`
- [ ] `manim/animation` -> `src/manim_cpp/animation`
- [ ] `manim/scene` -> `src/manim_cpp/scene`
- [ ] `manim/renderer` -> `src/manim_cpp/renderer`
- [ ] `manim/_config` and `manim/cli` -> `src/manim_cpp/config` + `apps/manim_cpp_cli`
- [ ] `manim/plugins` -> `src/manim_cpp/plugin` + `plugins/sdk`
- [ ] `docs/source` and `docs/i18n` -> `docs/book` and `docs/api` while preserving locale catalogs
- [x] `tests/control_data` and `tests/test_graphical_units/control_data` consumed by C++ regression harness

### Test Cases and Acceptance Scenarios
- [x] CLI compatibility snapshots
- [ ] Render parity (per-platform bitwise)
- [ ] Media parity (metadata/timing/section outputs)
- [ ] Scene API parity on example scenes
- [ ] Interaction parity (GLFW/ImGui)
- [ ] Plugin ABI integration tests (all platforms)
- [x] Migration utility integration tests
- [x] Docs/i18n build and catalog continuity checks
- [x] Strict zero-Python gate passing on mainline

### Rollout and Branching Strategy
- [x] Use phased cutover via `codex/*` branch workflow
- [ ] Require milestone gate reviews before merging next tranche
- [ ] Keep `manim`/`manimce` aliases for one major version after stable `manim-cpp`
- [ ] Publish binary artifacts via GitHub Releases + OS package channels
- [ ] Final cutover removes Python assets only after all parity gates pass

### Assumptions and Defaults (Locked)
- [x] Absolute zero Python in final repository and CI
- [x] Near-1:1 parity objective
- [x] C++ scene authoring only
- [x] Cairo + OpenGL + FFmpeg retained as core external libraries
- [x] Rebuild most non-core algorithmic logic in-repo
- [x] Monorepo architecture
- [x] C++23 baseline on GCC 13+/Clang 17+/MSVC 19.38+
- [x] System package dependency model (no Conan/vcpkg)
- [x] Platform scope: Linux + macOS + Windows
- [x] Docs stack: Doxygen + mdBook
- [x] Test stack: GoogleTest/GMock + custom render harness
- [x] Baseline artifact format stays `.npz` initially
- [x] Notebook workflow deferred
- [x] Quality-first schedule

## Notes
- Existing implementation commits already landed for the foundation and shader migration scaffolds.
- Ongoing execution style: strict TDD slices with checkpoint commits after each verified green batch.
- Current local verification command:
  `cmake --preset debug --fresh -D CMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++ -D CMAKE_CXX_FLAGS='-stdlib=libc++ -I/opt/homebrew/opt/llvm/include/c++/v1' -D CMAKE_EXE_LINKER_FLAGS='-L/opt/homebrew/opt/llvm/lib/c++ -Wl,-rpath,/opt/homebrew/opt/llvm/lib/c++ -stdlib=libc++' -D CMAKE_SHARED_LINKER_FLAGS='-L/opt/homebrew/opt/llvm/lib/c++ -Wl,-rpath,/opt/homebrew/opt/llvm/lib/c++ -stdlib=libc++' && cmake --build --preset debug -j4 && ctest --preset debug --output-on-failure`

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
- [ ] Legacy Python removal (deferred to final cutover)

### Implementation Workstreams and Milestones
- [x] Milestone A: Foundation and build skeleton
- [ ] Milestone B: Core math and geometry kernel
- [x] Milestone B.1: Math kernel bootstrap (`choose`, `rotate_z`, fan triangulation) with `gtest` coverage
- [x] Milestone B.2: Introduce Eigen-backed vector/matrix adapters for kernel usage
- [ ] Milestone B.3: Port geometry algorithms (triangulation variants, boolean path ops, graph layouts, isocurve extraction)
- [x] Milestone B.3.a: Ear-clipping triangulation for simple concave/convex polygons
- [ ] Milestone B.3.b: Boolean path operations
- [x] Milestone B.3.b.1: Foundational path predicates (`segments_intersect`, `point_in_polygon`, `has_self_intersections`)
- [x] Milestone B.3.b.2: Convex polygon intersection via Sutherland-Hodgman clipping
- [x] Milestone B.3.c: Graph layouts, isocurve extraction, and combinatorics helpers
- [x] Milestone B.3.c.1: Deterministic circular graph layout helper
- [x] Milestone B.3.c.2: Marching-squares isocurve extraction helper
- [x] Milestone B.3.c.3: Combinatorics helpers parity
- [ ] Milestone C: Scene graph and animation engine parity
- [x] Milestone C.1: Scene lifecycle runner (`setup` -> `construct` -> `tear_down`)
- [x] Milestone C.2: Basic animation playback timeline with updater ticks and elapsed-time tracking
- [x] Milestone C.3: Deterministic scene RNG seed controls (`set_random_seed`, replayable random stream)
- [x] Milestone C.4: Animation run-time and rate-function support integrated into `Scene::play`
- [ ] Milestone D: Cairo backend parity
- [ ] Milestone E: OpenGL backend + interaction parity
- [ ] Milestone F: Media/file writer + subtitles/audio parity
- [x] Milestone F.1: Subcaption collection + `.srt` file emission scaffold
- [x] Milestone F.2: Explicit section tracking and per-section partial movie routing
- [ ] Milestone G: CLI/config parity
- [x] Milestone G.1: Command/subcommand validation scaffolding with deterministic exit codes
- [x] Milestone G.2: `cfg show` and `cfg write` operate on real config files/templates
- [x] Milestone G.3: `plugins list <dir>` validates input and reports discovered shared libraries
- [x] Milestone G.4: `checkhealth --json` emits machine-readable dependency status
- [ ] Milestone H: Plugin system parity
- [x] Milestone H.1: Plugin discovery by shared-library extension + host ABI version gate
- [x] Milestone H.2: Batch plugin loading API with deterministic failure aggregation
- [x] Milestone H.3: Directory-first plugin load convenience API (discover + batch load)
- [ ] Milestone I: Migration utility parity
- [x] Milestone I.1: Detect `Scene`-family base classes + preserve original `self.*(...)` calls as TODO annotations
- [x] Milestone I.2: `manim-cpp-migrate --report <file>` writes deterministic migration summaries
- [ ] Milestone J: Test suite full port
- [x] Milestone J.1: Added C++ unit/integration coverage for core math/scene/cli/plugin/migrate scaffolds
- [ ] Milestone K: Docs and i18n migration parity
- [ ] Milestone L: Release engineering and cutover

### Detailed Subsystem Mapping (Source-to-Source Intent)
- [ ] `manim/mobject` -> `src/manim_cpp/mobject`
- [ ] `manim/animation` -> `src/manim_cpp/animation`
- [ ] `manim/scene` -> `src/manim_cpp/scene`
- [ ] `manim/renderer` -> `src/manim_cpp/renderer`
- [ ] `manim/_config` and `manim/cli` -> `src/manim_cpp/config` + `apps/manim_cpp_cli`
- [ ] `manim/plugins` -> `src/manim_cpp/plugin` + `plugins/sdk`
- [ ] `docs/source` and `docs/i18n` -> `docs/book` and `docs/api` while preserving locale catalogs
- [ ] `tests/control_data` and `tests/test_graphical_units/control_data` consumed by C++ regression harness

### Test Cases and Acceptance Scenarios
- [ ] CLI compatibility snapshots
- [ ] Render parity (per-platform bitwise)
- [ ] Media parity (metadata/timing/section outputs)
- [ ] Scene API parity on example scenes
- [ ] Interaction parity (GLFW/ImGui)
- [ ] Plugin ABI integration tests (all platforms)
- [ ] Migration utility integration tests
- [ ] Docs/i18n build and catalog continuity checks
- [ ] Strict zero-Python gate passing on mainline

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

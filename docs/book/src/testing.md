# Testing and Gates

`manim-cpp` uses GoogleTest/GMock plus repository guard scripts registered in
CTest.

## Core Commands

```sh
cmake --preset debug
cmake --build --preset debug -j4
ctest --preset debug --output-on-failure
```

## Important Gate Categories

- Render regression and bitwise baseline checks.
- CLI compatibility and alias contract checks.
- Plugin ABI loading and workflow enforcement checks.
- Zero-Python and release/workflow policy checks.
- Docs/i18n extraction and coverage checks.
- Original-Manim feature cross-test coverage matrix checks.

All gates are expected to pass before merging to `main`.

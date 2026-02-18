# Manim-Cpp Book

`manim-cpp` is a C++23 animation engine monorepo with a parity-oriented path from
Manim CE semantics to compiled C++ scene authoring.

This book documents how to build, run, test, and extend the repository after the
Python cutover.

## What This Book Covers

- System installation and toolchain requirements.
- First-scene authoring and rendering with `manim-cpp`.
- CLI command behavior and config layering.
- Plugin ABI and extension workflow.
- Migration workflow from legacy `.py` scenes.
- Testing and regression gates used in CI.

## Project Status

The mainline branch is Python-free and enforces deterministic render/media output
contracts through C++ tests and repository guard scripts.

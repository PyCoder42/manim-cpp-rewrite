# Installation

`manim-cpp` targets C++23 and requires platform packages for rendering and media
processing.

## Toolchain Floor

- GCC 13+
- Clang 17+
- MSVC 19.38+
- CMake 3.26+

## Install Dependencies

- Linux: `./tools/deps/install_linux.sh`
- macOS: `./tools/deps/install_macos.sh`
- Windows: `./tools/deps/install_windows.ps1`

## Configure and Build

```sh
cmake --preset debug
cmake --build --preset debug -j4
```

## Verify

```sh
ctest --preset debug --output-on-failure
```

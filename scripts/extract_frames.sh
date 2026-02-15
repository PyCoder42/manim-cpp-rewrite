#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 2 ]]; then
  echo "Manim-Cpp Graphical Test Frame Extractor"
  echo "usage: extract_frames.sh <input.npz> <output_directory>"
  exit 2
fi

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "${script_dir}/.." && pwd)"

if command -v manim-cpp-extract-frames >/dev/null 2>&1; then
  exec manim-cpp-extract-frames "$1" "$2"
fi

fallback_bin="${repo_root}/build/debug/apps/manim_cpp_extract_frames/manim-cpp-extract-frames"
if [[ -x "${fallback_bin}" ]]; then
  exec "${fallback_bin}" "$1" "$2"
fi

echo "Unable to find manim-cpp-extract-frames on PATH or in build/debug."
echo "Build the project first: cmake --build --preset debug"
exit 1

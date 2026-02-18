#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_toolchain_floor_contract.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
cmake_file="${repo_root}/CMakeLists.txt"
readme_file="${repo_root}/README.md"
install_doc="${repo_root}/docs/book/src/installation.md"

required_cmake_patterns=(
  "requires GCC 13+"
  "requires Clang 17+"
  "requires AppleClang 17+"
  "requires MSVC 19.38+"
)

for pattern in "${required_cmake_patterns[@]}"; do
  if ! rg -n --fixed-strings "${pattern}" "${cmake_file}" >/dev/null 2>&1; then
    echo "Missing toolchain floor pattern '${pattern}' in ${cmake_file}" >&2
    exit 1
  fi
done

if ! rg -n --fixed-strings "GCC 13+" "${readme_file}" >/dev/null 2>&1; then
  echo "README is missing GCC 13+ toolchain floor." >&2
  exit 1
fi

if ! rg -n --fixed-strings "Clang 17+" "${readme_file}" >/dev/null 2>&1; then
  echo "README is missing Clang 17+ toolchain floor." >&2
  exit 1
fi

if ! rg -n --fixed-strings "MSVC 19.38+" "${readme_file}" >/dev/null 2>&1; then
  echo "README is missing MSVC 19.38+ toolchain floor." >&2
  exit 1
fi

for pattern in "GCC 13+" "Clang 17+" "MSVC 19.38+"; do
  if ! rg -n --fixed-strings "${pattern}" "${install_doc}" >/dev/null 2>&1; then
    echo "Installation docs are missing toolchain floor '${pattern}'." >&2
    exit 1
  fi
done

echo "Toolchain floor contract verified across CMake and docs."

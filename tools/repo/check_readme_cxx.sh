#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_readme_cxx.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
readme_path="${repo_root}/README.md"

if [[ ! -f "${readme_path}" ]]; then
  echo "Missing README: ${readme_path}" >&2
  exit 1
fi

required_patterns=(
  "manim-cpp"
  "C++23"
  "cmake --preset"
  "apps/manim_cpp_cli"
)

for pattern in "${required_patterns[@]}"; do
  if ! rg -n --fixed-strings -- "${pattern}" "${readme_path}" >/dev/null 2>&1; then
    echo "README missing required C++ project guidance: ${pattern}" >&2
    exit 1
  fi
done

forbidden_patterns=(
  "from manim import"
  "pip install"
  "PyPI"
  "uv run python"
)

for pattern in "${forbidden_patterns[@]}"; do
  if rg -n --fixed-strings -- "${pattern}" "${readme_path}" >/dev/null 2>&1; then
    echo "README contains legacy Python-era guidance: ${pattern}" >&2
    exit 1
  fi
done

echo "README reflects C++ monorepo guidance."

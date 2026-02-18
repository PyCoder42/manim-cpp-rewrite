#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_final_cutover_gate.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
allowlist_path="${repo_root}/config/python_allowlist.txt"
plan_path="${repo_root}/docs/plans/2026-02-14-manim-cpp-full-repository-rewrite-plan.md"
tests_cmake_path="${repo_root}/tests_cpp/CMakeLists.txt"
python_gate_workflow="${repo_root}/.github/workflows/zero-python-gate.yml"

if [[ ! -f "${allowlist_path}" ]]; then
  echo "Missing allowlist file: ${allowlist_path}" >&2
  exit 1
fi

if [[ -s "${allowlist_path}" ]]; then
  echo "Python allowlist must be empty for final cutover." >&2
  exit 1
fi

if [[ ! -f "${tests_cmake_path}" ]]; then
  echo "Missing test CMake file: ${tests_cmake_path}" >&2
  exit 1
fi

if ! rg -n "test_bitwise_render_parity.cpp" "${tests_cmake_path}" >/dev/null 2>&1; then
  echo "Final cutover requires bitwise render parity regression coverage." >&2
  exit 1
fi

if ! rg -n "PluginAbiWorkflowCheck" "${tests_cmake_path}" >/dev/null 2>&1; then
  echo "Final cutover requires plugin ABI workflow enforcement coverage." >&2
  exit 1
fi

if [[ ! -f "${python_gate_workflow}" ]]; then
  echo "Missing zero-Python workflow: ${python_gate_workflow}" >&2
  exit 1
fi

if ! rg -n "Enforce zero Python when allowlist is empty" "${python_gate_workflow}" >/dev/null 2>&1; then
  echo "Zero-Python workflow must enforce strict mode when allowlist is empty." >&2
  exit 1
fi

if [[ ! -f "${plan_path}" ]]; then
  echo "Missing rewrite plan document: ${plan_path}" >&2
  exit 1
fi

required_plan_checks=(
  "- [x] Render parity (per-platform bitwise)"
  "- [x] Plugin ABI integration tests (all platforms)"
  "- [x] Strict zero-Python gate passing on mainline"
  "- [x] Final cutover removes Python assets only after all parity gates pass"
)

for check_line in "${required_plan_checks[@]}"; do
  if ! rg -n --fixed-strings -- "${check_line}" "${plan_path}" >/dev/null 2>&1; then
    echo "Missing checked final cutover gate in plan: ${check_line}" >&2
    exit 1
  fi
done

echo "Final cutover gate check passed."
